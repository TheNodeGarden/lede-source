#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/NG-NODEDON
        NAME:=NodeGarden NODEDON
        PACKAGES:=kmod-usb-core kmod-usb2 kmod-usb-storage
endef

define Profile/NG-NODEDON/Description
        Package set optimized for the NodeGarden NodEdoN.
endef

$(eval $(call Profile,NG-NODEDON))
