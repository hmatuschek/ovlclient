#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include <QListWidget>
#include "settings.hh"


class WhiteListView: public QWidget
{
  Q_OBJECT

public:
  WhiteListView(NodeIdList &whitelist, QWidget *parent=0);

public slots:
  void apply();

public:
  NodeIdList &_whitelist;
  QListWidget *_listview;
};


class SocksServiceSettingsView: public QWidget
{
  Q_OBJECT

public:
  SocksServiceSettingsView(SocksServiceSettings &settings, QWidget *parent=0);

public slots:
  void apply();

protected slots:
  void _onEnableToggled(bool value);
  void _onAllowBuddiesToggled(bool value);
  void _onAllowWhiteListToggled(bool value);

protected:
  SocksServiceSettings &_settings;
  QCheckBox *_enabled;
  QCheckBox *_allowBuddies;
  QCheckBox *_allowWhiteList;
  WhiteListView *_whitelist;
};


class UPNPSettingsView: public QWidget
{
  Q_OBJECT

public:
  UPNPSettingsView(UPNPSettings &settings, QWidget *parent=0);

public slots:
  void apply();

protected:
  UPNPSettings &_settings;
  QCheckBox *_enabled;
  QLineEdit *_externalPort;
};


class SettingsDialog : public QDialog
{
  Q_OBJECT

public:
  SettingsDialog(Settings &settings, QWidget *parent = 0);

public slots:
  void apply();

protected:
  Settings &_settings;
  SocksServiceSettingsView *_socks;
  UPNPSettingsView *_upnp;
};

#endif // SETTINGSDIALOG_H
