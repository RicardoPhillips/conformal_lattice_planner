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

#pragma once

#include <carla/client/Vehicle.h>
#include <carla/geom/Transform.h>

namespace planner {

/**
 * \brief Vehicle keeps tracks the information of a vehicle during the planning.
 */
class Vehicle {

protected:

  using CarlaBoundingBox = carla::geom::BoundingBox;
  using CarlaTransform   = carla::geom::Transform;
  using CarlaVehicle     = carla::client::Vehicle;

protected:

  /// ID of the vehicle in the carla simulator.
  size_t id_;

  /// Bounding box of the vehicle.
  CarlaBoundingBox bounding_box_;

  /// Transform of the vehicle, left handed to be compatible with the carla simualtor.
  CarlaTransform transform_;

  /// Speed of the vehicle.
  double speed_ = 0.0;

  /// The policy speed of this vehicle.
  double policy_speed_ = 0.0;

  /// Acceleration of the vehicle, brake should be negative.
  double acceleration_ = 0.0;

public:

  /**
   * The \c acceleration_ of the object won't be filled in using \c CarlaVehicle's
   * \c GetAcceleration() API, since the vehicles are assumed to be teleported
   * instead of controlled through physical dynamics.
   */
  Vehicle(const boost::shared_ptr<const CarlaVehicle>& actor, const double policy_speed) :
    id_          (actor->GetId()),
    bounding_box_(actor->GetBoundingBox()),
    transform_   (actor->GetTransform()),
    speed_       (actor->GetVelocity().Length()),
    policy_speed_(policy_speed),
    acceleration_(0.0) {}

  Vehicle(const size_t id,
          const CarlaBoundingBox& bounding_box,
          const CarlaTransform& transform,
          const double speed,
          const double policy_speed,
          const double acceleration) :
    id_          (id),
    bounding_box_(bounding_box),
    transform_   (transform),
    speed_       (speed),
    policy_speed_(policy_speed),
    acceleration_(acceleration) {}

  const size_t id() const { return id_; }
  size_t& id() { return id_; }

  const CarlaBoundingBox boundingBox() const { return bounding_box_; }
  CarlaBoundingBox& boundingBox() { return bounding_box_; }

  const CarlaTransform transform() const { return transform_; }
  CarlaTransform& transform() { return transform_; }

  const double speed() const { return speed_; }
  double& speed() { return speed_; }

  const double policySpeed() const { return policy_speed_; }
  double& policySpeed() { return policy_speed_; }

  const double acceleration() const { return acceleration_; }
  double& acceleration() { return acceleration_; }

  /**
   * \brief Update the vehicle in the simulator server.
   *
   * The function throws runtime error if the ID of input \c actor does not
   * match the ID of the object.
   *
   * The acceleration of the \c actor won't be set.
   *
   * \param[in] actor The vehicle to be updated.
   */
  void updateCarlaVehicle(const boost::shared_ptr<CarlaVehicle>& actor) const {
    if (actor->GetId() != id_)
      throw std::runtime_error("The vehicle ID does not match.");
    actor->SetTransform(transform_);
    actor->SetVelocity(transform_.GetForwardVector()*speed_);
    // No need to set the acceleration of the vehicle.
    return;
  }

  /**
   * \brief Get the vehicle ID, transform, and bounding box as a tuple.
   */
  std::tuple<size_t, CarlaTransform, CarlaBoundingBox> tuple() const {
    return std::make_tuple(id_, transform_, bounding_box_);
  }

  // TODO: Add a function to update the vehicle's transform and speed
  //       using the new acceleration and path.

}; // End class Vehicle.
} // End namespace planner.