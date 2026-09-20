if [ -e ./cm_app_build.config ]; then
    echo "cm_app配置成功"
else
    touch ./cm_app_build.config
    echo "请输入你的env绝对路径"
    echo "如： /home/xxx/openvela/myenv/bin/activate"
    read env_path
    echo "请输入你的xtensa工具链安装绝对路径"
    echo "如：/opt/xtensa-esp32s3-elf"
    read xtensa_path
    sudo touch cm_app_build.config
    echo -e "$env_path\n$xtensa_path" > cm_app_build.config
    echo "配置完毕"
    echo "如需重新配置，可执行remove.sh或删除openvela根目录下的cm_app_build.config文件后重新执行update.sh即可"
    cp -f ./contest2026_026_dami/tools/defconfig ./vendor/espressif/boards/esp32s3/esp32s3-eye/configs/openvela/
fi
