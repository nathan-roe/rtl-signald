#!/bin/bash

PROPERTIES_FILE=$HOME/.config/rtlsignald.properties
SERVICE_FILE=/etc/systemd/system/rtlsignald.service
# Install libudev dependencies (device events)
sudo apt install libudev-dev rtl-433
# Create config file
sudo rm $PROPERTIES_FILE 2> /dev/null
cat <<EOF > $PROPERTIES_FILE
signal_code=$RTL_SIGNALD_CODE
EOF
sudo chown $RTL_USER $PROPERTIES_FILE