/*
 * Copyright [2019] [Ke Sun]
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <iostream>
#include <vector>
#include <stdexcept>
#include <boost/timer/timer.hpp>
#include <boost/pointer_cast.hpp>
#include <conformal_lattice_planner/lane_follower.h>

using namespace std;
namespace bst = boost;
namespace cg = carla::geom;

namespace planner {

void LaneFollower::plan(const size_t target,
          const std::vector<size_t>& others) {

  // Get the target vehicle.
  SharedPtr<Vehicle> target_vehicle =
    bst::static_pointer_cast<Vehicle>(world_->GetActor(target));

  // Get the (desired) speed of the target vehicle.
  // TODO: Maybe the target should look ahead a bit for desired speed
  //       in order to avoid aggressive brake.
  // FIXME: It seems that the unit of speed limit is km/h.
  const double target_desired_speed = target_vehicle->GetSpeedLimit() / 3.6;
  SharedPtr<Waypoint> target_waypoint =
    map_->GetWaypoint(target_vehicle->GetTransform().location);
  //double target_speed = target_vehicle->GetVelocity().Length();
  printf("target location: x:%f y:%f z:%f r:%f p:%f y:%f\n",
      target_vehicle->GetTransform().location.x,
      target_vehicle->GetTransform().location.y,
      target_vehicle->GetTransform().location.z,
      target_vehicle->GetTransform().rotation.roll,
      target_vehicle->GetTransform().rotation.pitch,
      target_vehicle->GetTransform().rotation.yaw);
  printf("target speed: %f\n", target_speed_);
  printf("target desired speed: %f\n", target_desired_speed);

  // Get the leader vehicle.
  SharedPtr<Vehicle> lead_vehicle = nullptr;
  bst::optional<size_t> leader =  findLeader(target, others);
  if (leader) {
    lead_vehicle = bst::static_pointer_cast<Vehicle>(world_->GetActor(*leader));
  }

  // Compute the acceleration of the target vehicle
  // using the intelligent driver model.
  double target_accel = 0.0;
  if (lead_vehicle) {
    // Lead vehicle speed.
    const double lead_speed = lead_vehicle->GetVelocity().Length();
    // Following distance.
    // TODO: What if the lead vehicle is on a different section of road.
    SharedPtr<Waypoint> lead_waypoint =
      map_->GetWaypoint(lead_vehicle->GetTransform().location);
    const double following_distance = lead_waypoint->GetDistance() -
                                      target_waypoint->GetDistance();

    target_accel = idm_.idm(
        target_speed_, target_desired_speed, lead_speed, following_distance);

  } else {
    target_accel = idm_.idm(target_speed_, target_desired_speed);
  }
  printf("target idm accel: %f\n", target_accel);

  // Snap the target to the next closest waypoint.
  double distance_travelled = target_speed_*time_step_ + 0.5*target_accel*time_step_*time_step_;
  target_waypoint = findNextWaypoint(target_waypoint, distance_travelled);
  target_vehicle->SetTransform(target_waypoint->GetTransform());

  // Set the velocity of the target vehicle.
  // TODO: If the physics of the vehicles is disabled, is it still necessary to set the velocity?
  target_speed_ += target_accel * time_step_;
  //target_vehicle->SetVelocity(target_vehicle->GetTransform().GetForwardVector()*target_speed_);
  printf("updated target speed: %f\n", target_speed_);

  return;
}

LaneFollower::SharedPtr<LaneFollower::Waypoint>
  LaneFollower::findNextWaypoint(
    const SharedPtr<Waypoint>& waypoint, const double distance) {

  const cg::Vector3D waypoint_direction = waypoint->GetTransform().GetForwardVector();

  // Get candidate next waypoints at a certain distance from the given waypoint.
  std::vector<SharedPtr<Waypoint>> candidates = waypoint->GetNext(distance);
  SharedPtr<Waypoint> next_waypoint = nullptr;
  double best_score = -5.0;

  //std::printf("current waypoint:\n");
  //std::printf("IDs(road, section, lane): %d, %d, %d, transform: x:%f y:%f z:%f\n",
  //    waypoint->GetRoadId(),
  //    waypoint->GetSectionId(),
  //    waypoint->GetLaneId(),
  //    waypoint->GetTransform().location.x,
  //    waypoint->GetTransform().location.y,
  //    waypoint->GetTransform().location.z);

  // Loop through all candidates to find the one on the same lane.
  // TODO: What if road id or section id changes?
  // TODO: What happens if no candidate satisfies the requirement.
  //std::printf("candidate waypoints:\n");
  for (const auto& candidate : candidates) {
    //std::printf("IDs(road, section, lane): %d, %d, %d, transform: x:%f y:%f z:%f\n",
    //    candidate->GetRoadId(),
    //    candidate->GetSectionId(),
    //    candidate->GetLaneId(),
    //    candidate->GetTransform().location.x,
    //    candidate->GetTransform().location.y,
    //    candidate->GetTransform().location.z);

    // If the all IDs match, this is exactly the next waypoint we want.
    if (candidate->GetRoadId() == waypoint->GetRoadId() &&
        candidate->GetSectionId() == waypoint->GetSectionId() &&
        candidate->GetLaneId() == waypoint->GetLaneId()) {
      next_waypoint = candidate;
      break;
    }

    // If we cannot find the perfect match based on the IDs,
    // the forward direction of the waypoint is used.
    // The candidate whose forward direction matches the given waypoint
    // is the one we want.
    const cg::Vector3D candidate_direction = candidate->GetTransform().GetForwardVector();
    const double score =
      waypoint_direction.x*candidate_direction.x +
      waypoint_direction.y*candidate_direction.y +
      waypoint_direction.z*candidate_direction.z;

    if (score > best_score) {
      best_score = score;
      next_waypoint = candidate;
    }
  }

  if (!next_waypoint) {
    throw std::runtime_error("Cannot find next waypoint.");
  }

  return next_waypoint;
}

} // End namespace planner.