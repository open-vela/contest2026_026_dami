if [ -e ./cm_app_build.config ]; then

    CM_PATH="./contest2026_026_camera_app/Kconfig"
    env_path=$(sed -n '1p' cm_app_build.config)
    xtensa_path=$(sed -n '2p' cm_app_build.config)
    
    source $env_path
    export PATH="$xtensa_path/bin:$PATH"

    rm nuttx/.config
    rm nuttx/Make.defs
    
    ./build.sh vendor/espressif/boards/esp32s3/esp32s3-eye/configs/openvela/ -j8
    
    if grep -q "contest2026_026_camera_app" ./packages/Kconfig; then
        echo "cm_app has been added to the Kconfig system"
    else
        sed -i '/endmenu # Packages/i source "$CM_PATH"' ./packages/Kconfig
        echo "cm_app添加成功"
    fi
    

    #./build.sh vendor/espressif/boards/esp32s3/esp32s3-eye/configs/openvela/ menuconfig
    
    cd nuttx
    make -j8 EXTRAFLAGS="-Wno-cpp -Wno-deprecated-declarations"
    cd ..

    deactivate
else
    echo "请先执行update.sh进行项目配置更新"
    echo "如：sudo bash ./contest2026_026_dami/tools/update.sh"
fi

