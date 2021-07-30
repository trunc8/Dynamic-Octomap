# Dynamic Octomap

### Pre-requisites
- ROS Melodic installation with Gazebo9
- Packages installed: `ros-melodic-turtlebot3-*`
- ROS workspace at home directory: `~/catkin_ws`

### Demo
![Dynamic Octomap Demo](dynamic-octomap-demo.gif)

### Steps to Replicate

##### Setting up ROS Packages
```sh
cd ~/catkin_ws/src
git clone git@github.com:trunc8/Dynamic-Octomap.git
cd Dynamic-Octomap/octomap_server
rosdep install --from-paths . --ignore-src --rosdistro melodic -y
cd ~/catkin_ws
catkin build -j3
```

##### Octomap Library
```sh
cd ~
git clone -b library git@github.com:trunc8/Dynamic-Octomap.git
cd Dynamic-Octomap/octomap/
mkdir build && cd build
cmake ..
sudo make install
sudo cp ../lib/liboctomap.* /opt/ros/melodic/lib/
```

##### Running the demonstration
```sh
### Optional steps commented below
### You should ideally have these in your .bashrc or .zshrc
# export TURTLEBOT3_MODEL=waffle
# source /opt/ros/melodic/setup.sh
# source ~/catkin_ws/devel/setup.sh
roslaunch demonstrations deduct_dynamic_demo.launch
```

### Author(s)

* **Siddharth Saha** - [trunc8](https://github.com/trunc8)
* [octomap Library](https://github.com/OctoMap/octomap)
* [octomap_mapping ROS Package](https://github.com/OctoMap/octomap_mapping)

<p align='center'>Created with :heart: by <a href="https://www.linkedin.com/in/sahasiddharth611/">Siddharth</a></p>