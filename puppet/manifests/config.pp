# Private class
class echoes_alert_api::config inherits echoes_alert_api {
  if $caller_module_name != $module_name {
    fail("Use of private class ${name} by ${caller_module_name}")
  }

}
