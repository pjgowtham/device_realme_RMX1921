#!/system/bin/sh
if ! applypatch --check EMMC:/dev/block/bootdevice/by-name/recovery:67108864:6f95cb9c5f2d622d8367b07b4576d89a3507501b; then
  applypatch  \
          --patch /vendor/recovery-from-boot.p \
          --source EMMC:/dev/block/bootdevice/by-name/boot:67108864:1bfc8cf576ab3168eb0894e73a0463286206d4c0 \
          --target EMMC:/dev/block/bootdevice/by-name/recovery:67108864:6f95cb9c5f2d622d8367b07b4576d89a3507501b && \
      log -t recovery "Installing new oppo recovery image: succeeded" && \
      setprop ro.recovery.updated true || \
      log -t recovery "Installing new oppo recovery image: failed" && \
      setprop ro.recovery.updated false
else
  log -t recovery "Recovery image already installed"
  setprop ro.recovery.updated true
fi
