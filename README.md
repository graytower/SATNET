# SATNET
A system for satellite topology emulation writed by C++

## Introduction:
The development and launch costs of satellite routers are high, and it is especially important to verify large-scale ground emulation networks and semi-physical networks before launching. Compared with traditional network emulation solutions, the ground emulation of ultra-large-scale satellite constellation networking has the characteristics of high dynamic network, large-scale networking and real-life semi-physical emulation. Aiming at the above requirements, we propose a container-based low-orbit satellite network protocol emulation system combining discrete event simulation technology and virtualization emulation technology. In the control plane, the system is based on the mathematical modeling of the satellite network and generates discrete events of link up and down under the drive of the internal clock, which truly reflects the regular topology changes of the network. In the data plane, the system abstracts the emulation network based on Docker containers and Linux virtual network tools, and designs distributed deployment and semi-physical emulation solutions, which make the system have excellent performance horizontal scalability. Evaluation shows that our system can successfully simulate large number of nodes and discrete events.

## System:
The system includes seven files to emulate the topology in src packet: cmd.cpp, common.cpp, main.cpp, node.cpp, route.cpp, timer.cpp, topo.cpp; two configuration files related with the constellation topology and inter-satellite link: linktable.txt, orbit.bin ; one executable program in del packet to delete all virtual devices forcibly:del ; one text files to record the system operation logs: log.txt; one text files to compile all files.: Makefile; and one executable program to run the system: main

### ./main
The executable program to run the system.

### src
This packet includes seven cpp files, which could emulate the satellite network.

cmd.cpp
Encapsulate related commands to add/delete virtual network devices and add/delete/update the route/port status on virtual network devices.

common.cpp
Include and initialize global variables used in the system.

main.cpp
The main process to start/stop/exit the emulation system.

node.cpp
Encapsulate node actions and node topological relationships.

route.cpp
Realize the Dijkstra algorithm to calculate the next closest node for each satellite node, and update related routes.
timer.cpp
Realize a timer in system.

topo.cpp
Initialize/Finalize/update the topology through reading configuration files and invoke corresponding node actions.

include
This packet includes corresponding header files of the cpp files in src packet.

### del 
This packet include one executable program to delete all virtual devices forcibly.

./del 
Delete all virtual devices and initialize the system forcibly.

### topofile
Include two configuration files related with the constellation topology and inter-satellite link.

orbit.bin
A binary file including the latitude and longitude of all satellites for every second of the day.

linktable.txt
A text file record the IP address of each inter-satellite network and its corresponding satellite number and port number.

### log.txt
Record the system operation log that when the satellite topology will change.

### Makefile
Compile all cpp files in src packet.

## How to run
### First,enter the root direction of the system.
cd SATNET

### Second,run the system.
Sudo ./main

### Third,enter “start” to start the timer and wait the route be configured of each node 

### You can verify the satellite emulation network by entering some commands to examine the container running state

('''
sudo ip netns show
''')

('''
sudo ip netns exec sat1 ip route show
''')

### Also, you can verify the network connectivity by entering some commands:
('''
sudo ip netns exec sat1 ping 3.0.0.2
''')

### You can stop the emulation system by entering "stop" and exit the system by entering "exit"

### Once the virtual network devices are not completely cleared, you can clear them forcibly by entering:
('''
sudo del/./del
''')
