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

#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>

#include <tf2_ros/transform_broadcaster.h>
#include <geometry_msgs/TransformStamped.h>
#include <visualization_msgs/Marker.h>
#include <visualization_msgs/MarkerArray.h>
#include <sensor_msgs/Image.h>

#include <carla/client/Vehicle.h>
#include <carla/client/Map.h>
#include <carla/client/Waypoint.h>
#include <carla/geom/Transform.h>
#include <carla/sensor/data/Image.h>

#include <router/loop_router/loop_router.h>
#include <planner/common/waypoint_lattice.h>
#include <planner/common/traffic_lattice.h>
#include <planner/common/traffic_manager.h>
#include <planner/common/utils.h>
#include <planner/idm_lattice_planner/idm_lattice_planner.h>
#include <planner/spatiotemporal_lattice_planner/spatiotemporal_lattice_planner.h>

namespace node {

sensor_msgs::ImagePtr createImageMsg(
    const boost::shared_ptr<const carla::sensor::data::Image>&);

visualization_msgs::MarkerPtr createWaypointMsg(
    const std::vector<boost::shared_ptr<const carla::client::Waypoint>>&);

visualization_msgs::MarkerPtr createJunctionMsg(
    const carla::client::Map::TopologyList&);

visualization_msgs::MarkerArrayPtr createRoadIdsMsg(
    const std::unordered_map<uint32_t, carla::road::Road>&);

visualization_msgs::MarkerArrayPtr createVehiclesMsg(
    const std::vector<boost::shared_ptr<const carla::client::Vehicle>>&);

visualization_msgs::MarkerArrayPtr createVehicleIdsMsg(
    const std::vector<boost::shared_ptr<const carla::client::Vehicle>>&);

geometry_msgs::TransformStampedPtr createVehicleTransformMsg(
    const boost::shared_ptr<const carla::client::Vehicle>&, const std::string&);

visualization_msgs::MarkerArrayPtr createWaypointLatticeMsg(
    const boost::shared_ptr<const planner::WaypointLattice>&);

visualization_msgs::MarkerArrayPtr createTrafficLatticeMsg(
    const boost::shared_ptr<const planner::TrafficLattice>&);

visualization_msgs::MarkerArrayPtr createTrafficManagerMsg(
    const boost::shared_ptr<const planner::TrafficManager>&);

template<typename Path>
void populatePathMsg(const Path& path, const visualization_msgs::MarkerPtr& path_msg) {

  std::vector<std::pair<carla::geom::Transform, double>>
    transforms = path.samples();

  for (auto& transform : transforms) {
    utils::convertTransformInPlace(transform.first);
    geometry_msgs::Point pt;
    pt.x = transform.first.location.x;
    pt.y = transform.first.location.y;
    pt.z = transform.first.location.z;

    path_msg->points.push_back(pt);
    path_msg->colors.push_back(path_msg->color);
  }

  return;
}

template<typename Path>
visualization_msgs::MarkerPtr createEgoPathMsg(const Path& path) {

  visualization_msgs::MarkerPtr path_msg(new visualization_msgs::Marker);

  std_msgs::ColorRGBA path_color;
  path_color.r = 0.4;
  path_color.g = 1.0;
  path_color.b = 0.4;
  path_color.a = 1.0;

  path_msg->header.stamp = ros::Time::now();
  path_msg->header.frame_id = "map";
  path_msg->ns = "ego_path";
  path_msg->id = 0;
  path_msg->type = visualization_msgs::Marker::LINE_STRIP;
  path_msg->action = visualization_msgs::Marker::ADD;
  path_msg->lifetime = ros::Duration(0.0);
  path_msg->frame_locked = false;
  path_msg->pose.orientation.w = 1.0;
  path_msg->scale.x = 1.8;
  path_msg->scale.y = 1.8;
  path_msg->scale.z = 1.8;
  path_msg->color = path_color;

  populatePathMsg(path, path_msg);
  return path_msg;
}

visualization_msgs::MarkerArrayPtr createConformalLatticeMsg(
    const boost::shared_ptr<const planner::IDMLatticePlanner>&);

visualization_msgs::MarkerArrayPtr createSpatiotemporalLatticeMsg(
    const boost::shared_ptr<const planner::SpatiotemporalLatticePlanner>&);

} // End namespace node.
