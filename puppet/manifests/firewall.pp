# Private class
class echoes_alert_api::firewall inherits echoes_alert_api {
  if $caller_module_name != $module_name {
    fail("Use of private class ${name} by ${caller_module_name}")
  }

  if $echoes_alert_api::manage_firewall {
    if defined('::firewall') {
      if $echoes_alert_api::https {
        firewall { '101 allow API HTTPs access':
          port  => [ $echoes_alert_api::https_port ],
          proto => 'tcp',
          jump  => 'allowed',
        }->
        firewall { '101 allow API HTTPs access - IPv6':
          port     => [ $echoes_alert_api::https_port ],
          proto    => 'tcp',
          jump     => 'allowed',
          provider => 'ip6tables',
        }
      }

      firewall { '100 allow API HTTP access':
        port  => [ $echoes_alert_api::http_port ],
        proto => 'tcp',
        jump  => 'allowed',
      }->
      firewall { '100 allow API HTTP access - IPv6':
        port     => [ $echoes_alert_api::http_port ],
        proto    => 'tcp',
        jump     => 'allowed',
        provider => 'ip6tables',
      }
    }
  }
}
