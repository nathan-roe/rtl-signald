#!/bin/bash

sudo rm $SERVICE_FILE 2> /dev/null
cat <<EOF > $SERVICE_FILE
[Unit]
Description=RTL_SIGNALD
After=network.target

[Service]
Type=simple
ExecStart=$RTL_SIGNALD_PATH --command '$RTL_SIGNALD_COMMAND' --require-code
Restart=on-failure
User=$RTL_USER
Group=$RTL_GROUP

[Install]
WantedBy=multi-user.target
EOF

sudo systemctl daemon-reload
sudo systemctl stop rtlsignald.service
sudo systemctl restart rtlsignald.service
journalctl -u rtlsignald.service -f