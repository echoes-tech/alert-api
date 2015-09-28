# Private class
class echoes_alert_api::service inherits echoes_alert_api {
  if $caller_module_name != $module_name {
    fail("Use of private class ${name} by ${caller_module_name}")
  }

  $monit_file = "/etc/monit/conf.d/${echoes_alert_api::service_name}"

  wt::service { $echoes_alert_api::service_name:
    binary_path    => $echoes_alert_api::binary_path,
    cert_file      => $echoes_alert_api::cert_file,
    http_port      => $echoes_alert_api::http_port,
    https          => $echoes_alert_api::https,
    https_port     => $echoes_alert_api::https_port,
    priv_key_file  => $echoes_alert_api::priv_key_file,
    service_enable => $echoes_alert_api::service_enable,
    service_ensure => $echoes_alert_api::service_ensure,
    service_name   => $echoes_alert_api::service_name,
  }->
  monit::check { $echoes_alert_api::service_name:
    content => template("${module_name}/${echoes_alert_api::branch}/${echoes_alert_api::version}${monit_file}.erb"),
  }

}
