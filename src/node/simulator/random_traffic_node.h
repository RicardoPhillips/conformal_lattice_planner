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

#include <planner/common/traffic_manager.h>
#include <node/simulator/simulator_node.h>

namespace node {

class RandomTrafficNode : public SimulatorNode {

private:

  using Base = SimulatorNode;
  using This = RandomTrafficNode;

public:

  using Ptr = boost::shared_ptr<This>;
  using ConstPtr = boost::shared_ptr<const This>;

protected:

  using CarlaSensor = carla::client::Sensor;
  using CarlaSensorData = carla::sensor::SensorData;
  using CarlaBGRAImage = carla::sensor::data::Image;

protected:

  /// Traffic lattice.
  boost::shared_ptr<planner::TrafficManager> traffic_manager_;

  /// Nominal policy speed of all vehicles.
  const double nominal_policy_speed_ = 20.0;

public:

  RandomTrafficNode(ros::NodeHandle nh) : Base(nh) {}

protected:

  virtual void spawnVehicles() override;

  virtual boost::optional<size_t> spawnEgoVehicle(
      const boost::shared_ptr<const CarlaWaypoint>& waypoint,
      const double policy_speed,
      const bool noisy_speed = true) override;

  virtual boost::optional<size_t> spawnAgentVehicle(
      const boost::shared_ptr<const CarlaWaypoint>& waypoint,
      const double policy_speed,
      const bool noisy_speed = true) override;

  /// Manager (add/delete) the vehicles in the simulation.
  void manageTraffic();

  virtual void tickWorld() override;

  virtual void publishTraffic() const override;

}; // End class RandomTrafficNode.

using RandomTrafficNodePtr = RandomTrafficNode::Ptr;
using RandomTrafficNodeConstPtr = RandomTrafficNode::ConstPtr;

} // End namespace node.
