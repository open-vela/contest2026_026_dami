if [ -e ./cm_app_build.config ]; then
    env_path=$(sed -n '1p' cm_app_build.config)
    xtensa_path=$(sed -n '2p' cm_app_build.config)
    source $env_path
    export PATH="$xtensa_path/bin:$PATH"
    sudo chmod 666 /dev/ttyACM0
    cd nuttx
    make -j$(nproc) flash ESPTOOL_PORT=/dev/ttyACM0 ESPTOOL_BINDIR=./
    deactivate
    sudo minicom -D /dev/ttyACM0
else
    echo "请先执行update.sh进行项目配置更新"
    echo "如：sudo bash ./contest2026_026_dami/tools/update.sh"
fi
