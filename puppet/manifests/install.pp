# Private class
class echoes_alert_api::install inherits echoes_alert_api {
  if $caller_module_name != $module_name {
    fail("Use of private class ${name} by ${caller_module_name}")
  }

  if $echoes_alert_api::https {
    require openssl
  }

  file { $echoes_alert_api::install_dir:
    ensure => 'directory',
    owner  => 0,
    group  => 0,
    mode   => '0755'
  }->
  file { $echoes_alert_api::binary_path:
    ensure => 'directory',
    owner  => 0,
    group  => 0,
    mode   => '0755'
  }->
  file { "${echoes_alert_api::binary_path}/${echoes_alert_api::service_name}":
    ensure => 'file',
    owner  => 0,
    group  => 0,
    mode   => '0755',
    source => "puppet:///modules/${module_name}/${echoes_alert_api::branch}/${echoes_alert_api::version}/api",
  }->
  class { 'echoes_alert_probe':
    addons          => $echoes_alert_api::probe_addons,
    api_install_dir => $echoes_alert_api::install_dir,
    branch          => $echoes_alert_api::probe_branch,
    version         => $echoes_alert_api::probe_version,
  }

}
