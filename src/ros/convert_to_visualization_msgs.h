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
#include <sensor_msgs/Image.h>

#include <carla/client/Vehicle.h>
#include <carla/client/Map.h>
#include <carla/client/Waypoint.h>
#include <carla/geom/Transform.h>
#include <carla/sensor/data/Image.h>

#include <conformal_lattice_planner/loop_router.h>
#include <conformal_lattice_planner/waypoint_lattice.h>
#include <conformal_lattice_planner/traffic_lattice.h>
#include <conformal_lattice_planner/traffic_manager.h>

namespace carla {

sensor_msgs::ImagePtr createImageMsg(
    const boost::shared_ptr<const carla::sensor::data::Image>&);

visualization_msgs::MarkerPtr createWaypointMsg(
    const std::vector<boost::shared_ptr<const carla::client::Waypoint>>&);

visualization_msgs::MarkerPtr createJunctionMsg(
    const carla::client::Map::TopologyList&);

visualization_msgs::MarkerArrayPtr createRoadIdsMsg(
    const std::unordered_map<uint32_t, carla::road::Road>&);

//visualization_msgs::MarkerPtr createVehicleMsg(
//    const boost::shared_ptr<const cc::Vehicle>&);

visualization_msgs::MarkerArrayPtr createVehiclesMsg(
    const std::vector<boost::shared_ptr<const carla::client::Vehicle>>&);

visualization_msgs::MarkerArrayPtr createVehicleIdsMsg(
    const std::vector<boost::shared_ptr<const carla::client::Vehicle>>&);

//visualization_msgs::MarkerArrayPtr createVehicleIdsMsg(
//    const std::unordered_map<size_t, carla::geom::Transform>&);

geometry_msgs::TransformStampedPtr createVehicleTransformMsg(
    const boost::shared_ptr<const carla::client::Vehicle>&, const std::string&);

visualization_msgs::MarkerArrayPtr createWaypointLatticeMsg(
    const boost::shared_ptr<const planner::WaypointLattice<router::LoopRouter>>&);

visualization_msgs::MarkerArrayPtr createTrafficLatticeMsg(
    const boost::shared_ptr<const planner::TrafficLattice<router::LoopRouter>>&);

visualization_msgs::MarkerArrayPtr createTrafficManagerMsg(
    const boost::shared_ptr<const planner::TrafficManager<router::LoopRouter>>&);

} // End namespace carla.