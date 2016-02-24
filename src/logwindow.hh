#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include <QWidget>
#include <QAbstractTableModel>
#include <QTableView>
#include <ovlnet/logger.hh>

class Application;


class LogModel: public QAbstractTableModel, public LogHandler
{
  Q_OBJECT

public:
  explicit LogModel(QObject *parent = 0);
  virtual ~LogModel();

  void handleMessage(const LogMessage &msg);

  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;
  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;

protected:
  QVector<LogMessage> _messages;
};


class LogWidget: public QWidget
{
  Q_OBJECT

public:
  explicit LogWidget(Application &app);

protected:
  QTableView *_table;
};

#endif // LOGWINDOW_H
