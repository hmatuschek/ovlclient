#include "searchdialog.hh"
#include <ovlnet/dht_config.hh>

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QByteArray>
#include <QPushButton>
#include <QInputDialog>
#include <QTableWidgetItem>
#include <QCloseEvent>


SearchDialog::SearchDialog(Node *dht, BuddyList *buddies, QWidget *parent)
  : QWidget(parent), _dht(dht), _buddies(buddies)
{
  setWindowTitle(tr("Overlay network node search"));
  setMinimumWidth(600);
  _query = new QLineEdit();

  _result = new QTableWidget();
  _result->setColumnCount(3);
  QStringList headers; headers << tr("Identifier") << tr("Address") << tr("Port");
  _result->setHorizontalHeaderLabels(headers);

  QPushButton *addAsNew = new QPushButton(tr("Add as new contact"));
  QPushButton *addTo    = new QPushButton(tr("Add to contact"));

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(new QLabel(tr("Search for a node (identifier):")));
  layout->addWidget(_query, 0);
  QHBoxLayout *bbox = new QHBoxLayout();
  bbox->addWidget(addAsNew);
  bbox->addWidget(addTo);
  layout->addLayout(bbox, 0);
  layout->addWidget(_result, 1);
  setLayout(layout);

  QObject::connect(_query, SIGNAL(returnPressed()), this, SLOT(_onStartSearch()));
  QObject::connect(addAsNew, SIGNAL(clicked()), this, SLOT(_onAddAsNewBuddy()));
  QObject::connect(addTo, SIGNAL(clicked()), this, SLOT(_onAddToBuddy()));
  QObject::connect(_dht, SIGNAL(nodeFound(NodeItem)), this, SLOT(_onSearchSuccess(NodeItem)));
  QObject::connect(_dht, SIGNAL(nodeNotFound(Identifier,QList<NodeItem>)),
                   this, SLOT(_onSearchFailed(Identifier,QList<NodeItem>)));
}

void
SearchDialog::_onStartSearch() {
  // Assemble ID
  Identifier id = Identifier::fromBase32(_query->text());
  _currentSearch = id;
  _result->setRowCount(0);
  _dht->findNode(_currentSearch);
}

void
SearchDialog::_onAddAsNewBuddy() {
  // Get selected nodes
  QSet<Identifier> nodes;
  QList<QTableWidgetItem *> selectedItems = _result->selectedItems();
  QList<QTableWidgetItem *>::iterator item = selectedItems.begin();
  for (; item != selectedItems.end(); item++) {
    nodes.insert(
          Identifier::fromBase32(_result->item((*item)->row(), 0)->text()));
  }
  if (0 == nodes.size()) { return; }
  Identifier node = *(nodes.begin());

  // Get buddy name
  QString name = QInputDialog::getText(0, tr("Contact name"), tr("Select a name for the contact."));
  if (0 == name.size()) { return; }
  _buddies->addBuddy(name, node);
}

void
SearchDialog::_onAddToBuddy() {

}

void
SearchDialog::_onSearchSuccess(const NodeItem &node) {
  if (node.id() != _currentSearch) { return; }
  // Exclude myself from the results
  if (node.id() == _dht->id()) { return; }
  // Append a row with the results
  size_t idx = _result->rowCount();
  _result->setRowCount(idx+1);
  _result->setItem(idx, 0, new QTableWidgetItem(node.id().toBase32()));
  _result->setItem(idx, 1, new QTableWidgetItem(node.addr().toString()));
  _result->setItem(idx, 2, new QTableWidgetItem(QString::number(node.port())));
}

void
SearchDialog::_onSearchFailed(const Identifier &id, const QList<NodeItem> &best) {
  if (id != _currentSearch) { return; }
  QList<NodeItem>::const_iterator node = best.begin();
  for (; node != best.end(); node++) {
    // Exclude myself from list
    if (node->id() == _dht->id()) { continue; }
    // Append a row with the results
    size_t idx = _result->rowCount(); _result->setRowCount(idx+1);
    _result->setItem(idx, 0, new QTableWidgetItem(node->id().toBase32()));
    _result->setItem(idx, 1, new QTableWidgetItem(node->addr().toString()));
    _result->setItem(idx, 2, new QTableWidgetItem(QString::number(node->port())));
  }
}

void
SearchDialog::closeEvent(QCloseEvent *evt) {
  evt->accept();
  this->deleteLater();
}
