# Qualcomm power HAL for Fairphone 6

Source: Fairphone `platform/vendor/qcom-opensource/power`, revision
`2aa3974fc92a0d130e989a203b9a231398834071`. Upstream source history and notices
are preserved. Install this project at `vendor/qcom/opensource/power`.

The `android17` branch builds the common Volcano AIDL power service with Soong.
It retains the published implementation, interface version, service name and
init/VINTF fragments. The legacy Make definition is retained as
`Android.mk.legacy`; its unrelated SoC selection helpers are not evaluated.
The service consumes library-exported headers instead of an unused ICU include
path prohibited by the current Android build system.

The service loads `libqti-perfd-client.so` at runtime. The product must supply
and verify its performance-provider dependencies and device configuration;
compilation alone does not qualify power or thermal behavior.

Sources retain their individual BSD notices. `NOTICE` and `LICENSES` preserve
the relevant texts; the original file headers remain authoritative.
