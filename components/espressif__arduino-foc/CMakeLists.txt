set(SRC_FILES
    "src/common/pid.cpp"
    "src/common/time_utils.cpp"
    "src/common/lowpass_filter.cpp"
    "src/common/base_classes/CurrentSense.cpp"
    "src/common/base_classes/Sensor.cpp"
    "src/common/base_classes/FOCMotor.cpp"
    "src/communication/Commander.cpp"
    "src/communication/SimpleFOCDebug.cpp"
    "src/current_sense/GenericCurrentSense.cpp"
    "src/current_sense/LowsideCurrentSense.cpp"
    "src/sensors/GenericSensor.cpp"
    "src/sensors/Encoder.cpp"
    "src/sensors/HallSensor.cpp"
    "src/BLDCMotor.cpp")

set(INC_FILES "src/common/" "src/common/base_classes/" "src/")

set(IGNORE_FILES "src/sensors/Encoder.cpp" "src/sensors/HallSensor.cpp")

idf_component_register(SRCS ${SRC_FILES} INCLUDE_DIRS ${INC_FILES})

set_source_files_properties(
  ${IGNORE_FILES}
  PROPERTIES
    COMPILE_OPTIONS
    "-Wno-volatile;-Wno-deprecated-enum-float-conversion;-Wno-missing-field-initializers"
)
