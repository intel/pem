#
# Copyright (c) 2014, 2015 Intel Corporation.
#
# This program is free software; you can redistribute it and/or modify it
# under the terms and conditions of the GNU General Public License,
# version 2, as published by the Free Software Foundation.
#
# This program is distributed in the hope it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
# more details.
#

%define projectname pem

Name:       %{projectname}
Summary:    Peripherals EMulator
Version:    0.1.10
Release:    1
Group:      Development/Tools
License:    GPL-2.0
BuildArch:  noarch
Source:     %{name}-%{version}.tar.gz
Source100: %{name}-rpmlintrc

BuildRequires: python
BuildRequires: python-setuptools
BuildRequires: fdupes

Requires: python-setuptools
Requires: python-idle
Requires: python-tk
Requires: python-serial

%description
Tool for emulating peripherals.

%prep
%setup -q

%build
%{__python} setup.py build

%install
rm -rf %{buildroot}
install -d %{buildroot}/%{_datadir}/%{projectname}/pem_Arduino
install -m 0644 src/pem_Arduino/* %{buildroot}/%{_datadir}/%{projectname}/pem_Arduino/
%{__python} setup.py install -O2 --root=%{buildroot} --prefix=%{_prefix}
%fdupes %{buildroot}

%files
%defattr(-,root,root,-)
%{python_sitelib}/%{projectname}-%{version}-*.egg-info
%{python_sitelib}/%{projectname}
%{_datadir}/%{projectname}
%{_bindir}/%{name}

%changelog
