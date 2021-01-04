#!/bin/bash
set -e

REPO="$PWD"

YELLOW='\033[0;33m'
GREEN='\033[0;32m'
NC='\033[0m'

echo -e "${YELLOW}Generating bindings...${NC}"
docker-compose -f bridge/docker-compose.generate_bindings.yml up

echo -e "${YELLOW}Building...${NC}"
cd bridge
./build_darwin.sh
docker-compose up

echo -e "${YELLOW}WARNING!${NC} You will still have to build the Windows bridge manually"

echo -e "${GREEN}Done!${NC}"