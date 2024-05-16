include $(TOPDIR)/rules.mk

PKG_NAME:=tuya-esp
PKG_RELEASE:=1
PKG_VERSION:=1.0.0

PKG_BUILD_DEPENDS:=!USE_GLIBC:argp-standalone

include $(INCLUDE_DIR)/package.mk

define Package/tuya-esp
	CATEGORY:=Examples
	TITLE:=tuya-esp
	DEPENDS:= \
		+tuya-sdk-core \
		+libubus \
		+libubox \
		+libblobmsg-json
endef

define Package/tuya-esp/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tuya-esp $(1)/usr/bin
endef

$(eval $(call BuildPackage,tuya-esp))
