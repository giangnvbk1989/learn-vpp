#!/bin/bash

vpp=~/git/vpp-generic

# prep local build container
sudo docker rm -f vpp-generic &> /dev/null
sudo docker run -it \
  --privileged \
  --detach \
  --cap-add=ALL \
  --name vpp-generic \
  --user 1000 \
  -v $vpp:/opt/vpp \
  vpp-dev-1804:latest bash

sudo docker rm -f vpp-y &> /dev/null
sudo docker run -it \
  --privileged \
  --detach \
  --cap-add=ALL \
  --name vpp-y \
  -v $vpp:/opt/vpp \
  vpp-dev-1804:latest
sudo docker stop vpp-y
