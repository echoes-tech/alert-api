# echoes-alert-api

#### Table of Contents

1. [Overview](#overview)
2. [Module Description - What the module does and why it is useful](#module-description)
3. [Setup - The basics of getting started with wt](#setup)
    * [Setup requirements](#setup-requirements)
    * [Beginning with echoes_alert_api](#beginning-with-echoes_alert_api)
4. [Usage - Configuration options and additional functionality](#usage)
5. [Reference - An under-the-hood peek at what the module is doing and how](#reference)
6. [Limitations - OS compatibility, etc.](#limitations)
7. [Development - Guide for contributing to the module](#development)
8. [Contributors](#contributors)

## Overview

Puppet module to manage Echoes Alert API installation and configuration.

## Module Description

This module installs and configures Echoes Alert API.

## Setup

### Setup requirements

This module requires Wt to be installed.

You can use [echoes-wt](https://github.com/echoes-tech/puppet-wt) to install Wt.

The API resources (binaries, configuration files, etc.) should be located in the `files` or `templates` directories of the module, under the path `/$branch_name/$version_number/`.

### Beginning with echoes_alert_api

```puppet
include 'echoes_alert_api'
```

## Usage

An example of resource-like class declaration: 

```puppet
class {'echoes_alert_api':
  branch  => 'develop',
  version => 'latest',
}
```

## Reference

### Classes

#### Public classes

* echoes_alert_api: Main class, includes all other classes.

#### Private classes

* echoes_alert_api::params: Sets parameter defaults.
* echoes_alert_api::install: Handles the binary.
* echoes_alert_api::service: Handles the service.
* echoes_alert_api::firewall: Handles the firewall configuration.

#### Parameters

The following parameters are available in the `::echoes_alert_api` class:

##### `branch`

Tells Puppet which branch to choose to install API. Valid options: string. Default value: 'master'

##### `cert_file`

Specifies the certificate file to be used to launch the Wt server in https mode. Valid options: path. Default value: undef

##### `http_port`

Specifies the http port of the Wt server. Valid options: integer >= 1. Default value: 80

##### `https`

Tells Puppet whether the Wt server must be start with https mode. Valid options: boolean. Default value: false

##### `https_port`

Specifies the https port of the Wt server. Valid options: integer >= 1. Default value: 443

##### `manage_firewall`

If true and if puppetlabs-firewall module is present, Puppet manages firewall to allow HTTP access for Echoes Alert API. Valid options: 'true' or 'false'. Default value: 'false'

##### `priv_key_file`

Specifies the private key file to be used to launch the Wt server in https mode. Valid options: path. Default value: undef

##### `probe_addons`

Tells Puppet which Echoes Alert Probe addons to install. Valid options: hash. Default value: {}

##### `probe_branch`

Tells Puppet which branch to choose to install Probe. Valid options: string. Default value: 'master'

##### `probe_version`

Tells Puppet which version to choose to install Probe. Valid options: 'latest' or a specific version number. Default value: 'latest'

##### `servername`

Specifies the ServerName to be used to launch the Wt server. Valid options: string. Default value: echoes-alert-api

##### `service_ensure`

Tells Puppet whether the Echoes Alert API service should be running. Valid options: 'running' or 'stopped'. Default value: 'running'

##### `service_manage`

Tells Puppet whether to manage the Echoes Alert API service. Valid options: 'true' or 'false'. Default value: 'true'

##### `service_name`

Tells Puppet what Echoes Alert API service to manage. Valid options: string. Default value: 'ea-api'

##### `version`

Tells Puppet which version to choose to install API. Valid options: 'latest' or a specific version number. Default value: 'latest'

## Limitations

Debian family OSes is officially supported. Tested and built on Debian.

##Development

[Echoes Technologies](https://www.echoes-tech.com) modules on the Puppet Forge are open projects, and community contributions are essential for keeping them great.

## Contributors

The list of contributors can be found at: https://forge.echoes-tech.com/projects/api/repository/statistics#statistics-contributors

