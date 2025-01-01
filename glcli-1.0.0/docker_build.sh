#!/bin/bash

set -ex

LOCAL_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
USER_ID=$(id -u)
GROUP_ID=$(id -g)

if [ "$USER_ID" -eq 0 ]; then
    echo "This script cannot be run as root."
    exit 1
fi

if [ -e .cmd.sh ]; then
    rm .cmd.sh
fi
touch .cmd.sh
chmod +x .cmd.sh
cat <<EOF >> .cmd.sh
#!/bin/bash
set -ex

export PICOOS_BASE=$LOCAL_DIR/common/PicoOS
$@
EOF

pc805_docker_image=docker.picocom.com/pcomware/oru_pc805_build:v4

#docker login docker.picocom.com --username picocom --password picocom

docker_image=${pc805_docker_image}

docker run -t --user root  --entrypoint /bin/bash -v $LOCAL_DIR:$LOCAL_DIR \
     ${docker_image} \
     -l -c "groupadd -r -g $GROUP_ID docker_group && useradd -u $USER_ID docker_user -g $GROUP_ID  && \
     cd $LOCAL_DIR && su -c $LOCAL_DIR/.cmd.sh docker_user"

docker system prune -f || true
