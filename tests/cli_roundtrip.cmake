set(replay "${output}/roundtrip-replay.json")
set(snapshot "${output}/roundtrip-snapshot.json")
execute_process(COMMAND "${exe}" simulate "${scenario}" --wait 16 --until 32 --save-replay "${replay}" --save-snapshot "${snapshot}"
  RESULT_VARIABLE first_result OUTPUT_VARIABLE first_output ERROR_VARIABLE first_error)
if(NOT first_result EQUAL 0)
  message(FATAL_ERROR "simulate failed: ${first_error}")
endif()
execute_process(COMMAND "${exe}" replay "${replay}"
  RESULT_VARIABLE replay_result OUTPUT_VARIABLE replay_output ERROR_VARIABLE replay_error)
execute_process(COMMAND "${exe}" resume "${scenario}" "${snapshot}"
  RESULT_VARIABLE resume_result OUTPUT_VARIABLE resume_output ERROR_VARIABLE resume_error)
if(NOT replay_result EQUAL 0 OR NOT resume_result EQUAL 0 OR NOT replay_output STREQUAL resume_output)
  message(FATAL_ERROR "replay and restored snapshot differ: ${replay_error} ${resume_error}")
endif()
string(FIND "${first_output}" "\"time_ms\":32" checkpoint)
string(FIND "${replay_output}" "\"time_ms\":1600" finished)
if(checkpoint EQUAL -1 OR finished EQUAL -1)
  message(FATAL_ERROR "wrong checkpoint or replay duration")
endif()
