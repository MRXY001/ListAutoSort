#include "ListAutoSort.h"

ListAutoSort::ListAutoSort(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	// ��ʼ��ȫ�ֱ���
	rt = new RuntimeInfo();
	us = new USettings(rt->DATA_PATH + "settings.ini");
	savedPath = rt->DATA_PATH + "fields.txt";

	initView();
	readFIeldsInfo();
	setTableHeader();
}

/**
 * ��ʼ���ؼ�������
 */
void ListAutoSort::initView()
{
	ui.fieldsList->setEditTriggers(QAbstractItemView::DoubleClicked); // ����˫���༭

	// ���ô�������˫���༭�����¼�
	ListItemDelegate* delegate = new ListItemDelegate(this);
	ui.fieldsList->setItemDelegate(delegate);
	connect(delegate, SIGNAL(signalTextModified(int, QString)), this, SLOT(slotFieldItemTextModified(int, QString)));

	// �����źŲ�
	connect(ui.addCol, SIGNAL(clicked()), this, SLOT(slotFieldItemAdd()));
	connect(ui.inputButton, SIGNAL(clicked()), this, SLOT(slotInputButtonClicked()));
	connect(ui.insertButton, SIGNAL(clicked()), this, SLOT(slotInsertButtonClicked()));
	connect(ui.pasteButton, SIGNAL(clicked()), this, SLOT(slotPasteButtonClicked()));
	connect(ui.copyExcelButton, SIGNAL(clicked()), this, SLOT(slotExcelButtonClicked()));
	connect(ui.fieldsList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotFieldItemMenu(QPoint)));
	//connect(ui.fieldsList, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(slotFieldItemRowChanged())); // ��Ч
	//connect(ui.fieldsList, SIGNAL(itemActivated(QListWidgetItem *)), this, SLOT(slotFieldItemRowChanged())); // ��Ч
	connect(ui.fieldsList, SIGNAL(currentRowChanged(int)), this, SLOT(slotFieldItemRowChanged()));
	connect(ui.patternEdit, SIGNAL(textChanged(QString)), this, SLOT(slotFieldItemPatternModified(QString)));
}

/**
 * ��ʼ��������
 */
void ListAutoSort::setTableHeader()
{
	ui.tableWidget->setColumnCount(fields.size());
	QStringList labels;
	for (int i = 0; i < fields.size(); i++)
		labels.append(fields[i].getName());
	ui.tableWidget->setHorizontalHeaderLabels(labels);
}

/**
 * ��ĳһ�У����Ϸ�������ı������/���£�
 */
bool ListAutoSort::setTableARow(int row, QString mixture, QList<FieldItem>fields)
{
	QStringList result = analyzeMixture(mixture, fields);

	// ʶ��ʧ��
	if (result.size() != fields.size())
	{
		QString list_str = "";
		for (int i = 0; i < result.size(); i++)
		{
			if (i > 0) list_str += ", ";
			list_str += result[i];
		}
		QString all_msg = QString("Sorry, analyze failed, result is:%1").arg(list_str);
		QMessageBox::information(this, QStringLiteral("ʶ��ʧ��"), all_msg);
		return false;
	}

	// ��ӵ�����ĳһ��
	for (int i = 0; i < result.size(); i++)
	{
		QTableWidgetItem* item = new QTableWidgetItem(result[i]);
		ui.tableWidget->setItem(row, i, item);
	}

	return true;
}

/**
 * AIʶ��ķ��������� TableAITool��
 */
QStringList ListAutoSort::analyzeMixture(QString mixture, QList<FieldItem> fields)
{
	TableAITool ai(mixture, fields);

	return ai.getResult();
}

/**
 * �����ֶ�
 * * �б������ƶ�
 * * ��������ƶ�
 */
void ListAutoSort::moveListItem(int from, int to)
{
	if (from == to)
		return;

	// �ƶ� QList
	fields.move(from, to);
	writeFieldsInfo(); // ����λ���޸�

	// �ƶ� QListWidget
	ui.fieldsList->insertItem(to, new QListWidgetItem(ui.fieldsList->item(from)->text()));
	QListWidgetItem* item = ui.fieldsList->item(from + (from > to ? 1 : 0));
	ui.fieldsList->takeItem(from + (from > to ? 1 : 0));
	delete item;
	ui.fieldsList->setCurrentRow(to); // ����Ϊԭ����Item�ƶ��������

	// �ƶ� QTableWidget
	ui.tableWidget->insertColumn(to);
	if (from > to) from++;
	for (int i = 0; i < mixtures.size(); i++)
	{
		QTableWidgetItem* item = ui.tableWidget->item(i, from);
		ui.tableWidget->setItem(i, to, new QTableWidgetItem(*item));
	}
	ui.tableWidget->removeColumn(from);
	setTableHeader();
}

/**
 * ��������
 * * ��������ƶ�
 * * Mixtures�����ƶ�
 */
void ListAutoSort::moveTableItem(int from, int to)
{
	if (from == to)
		return;
	if (from < to) // ����
	{

	}
	else if (from > to) // ����
	{

	}
}

/*
 * �ӱ�����ļ��ж�ȡ
 */
void ListAutoSort::writeFieldsInfo()
{
	QString content;

	for (int i = 0; i < fields.size(); i++)
	{
		QString n = fields[i].getName();
		QString p = fields[i].getPattern();
		QString c = toXml(n, "name") + toXml(p, "pattern");
		content += toXml(c, "field");
	}

	writeTextFile(savedPath, content);
}

/**
 * ���浽�ļ�֮��
 */
void ListAutoSort::readFIeldsInfo()
{
	if (!isFileExist(savedPath))
		return;
	QString content = readTextFile(savedPath);

	QStringList ts = getXmls(content, "field");

	for (int i = 0; i < ts.size(); i++)
	{
		QString n = getXml(ts[i], "name");
		QString p = getXml(ts[i], "pattern");
		fields.append(FieldItem(n, p));
	}

	refreshFieldsInfo();
}

/**
 * ���˸���֮��ˢ���б�
 */
void ListAutoSort::refreshFieldsInfo()
{
	ui.fieldsList->clear();

	for (int i = 0; i < fields.size(); i++)
	{
		QListWidgetItem* item = new QListWidgetItem(fields[i].getName(), ui.fieldsList);
		item->setFlags(item->flags() | Qt::ItemIsEditable);
	}
}

/**
 * TableVIew���һ�У��������Ա༭
 */
void ListAutoSort::slotFieldItemAdd()
{
	// �����ֶ��б�
	QListWidgetItem* item = new QListWidgetItem(QStringLiteral("���ֶ�"), ui.fieldsList);
	item->setFlags(item->flags() | Qt::ItemIsEditable);
	ui.fieldsList->editItem(item);
	fields.append(FieldItem(QStringLiteral("���ֶ�"), ""));

	setTableHeader();

	// ����
	writeFieldsInfo();
}

/**
 * ��� ui.inputEdit �е����ݵ������
 */
void ListAutoSort::slotInputButtonClicked()
{
	QString mixture = ui.inputEdit->toPlainText();
	if (mixture.isEmpty()) return;

	mixtures.append(mixture);
	ui.tableWidget->setRowCount(mixtures.size()); // ������������������ͷ�У�
	if (setTableARow(mixtures.size() - 1, mixture, fields)) // �����е�����
	{
		ui.inputEdit->setPlainText("");
		ui.tableWidget->setCurrentCell(ui.tableWidget->rowCount() - 1, 0);
	}
	else // ����ʧ�ܣ�ȡ���޸�
	{
		ui.tableWidget->removeRow(ui.tableWidget->rowCount() - 1);
		mixture.remove(mixture.size());
	}
}

/**
 * ���� ui.inputEdit �е����ݵ������
 */
void ListAutoSort::slotInsertButtonClicked()
{
	int currentIndex = ui.tableWidget->currentRow();
	if (currentIndex < 0)
		return;

	QString mixture = ui.inputEdit->toPlainText();
	if (mixture.isEmpty()) return;

	mixtures.insert(currentIndex, mixture);
	ui.tableWidget->insertRow(currentIndex); // ͨ������һ����������������������ͷ�У�
	if (setTableARow(currentIndex, mixture, fields)) // �����е�����
	{
		ui.inputEdit->setPlainText("");
		ui.tableWidget->setCurrentCell(currentIndex, 0);
	}
	else // ����ʧ�ܣ�ȡ���޸�
	{
		ui.tableWidget->removeRow(ui.tableWidget->rowCount() - 1);
		mixture.remove(mixture.size());
	}
}

/**
 * ֱ����Ӽ������е����ݵ������
 */
void ListAutoSort::slotPasteButtonClicked()
{
	const QClipboard* clipboard = QApplication::clipboard();
	QString str = clipboard->text();
	ui.inputEdit->setPlainText(str);
	slotInputButtonClicked();
}

/**
 * ���Ʊ�������ΪExcel������+Tab
 */
void ListAutoSort::slotExcelButtonClicked()
{
	// ��������
	QString ans;
	for (int i = 0; i < mixtures.size(); i++)
	{
		for (int j = 0; j < fields.size(); j++)
		{
			if (j > 0) ans += "\t";
			ans += ui.tableWidget->item(i, j)->text();
		}
		ans += "\n";
	}
	QClipboard* clipboard = QApplication::clipboard();
	clipboard->setText(ans);

	// �Ƿ��г�����
	bool hasLongNumber = false;
	for (int i = 0; i < fields.size(); i++)
		if (canRegExp(fields[i].getPattern(), ""))
		{
			hasLongNumber = true;
			break;
		}

	QString tip = QStringLiteral("%1��%2�� ������ϣ��뵽Excel��Ӧ��λ��ճ��").arg(mixtures.size()).arg(fields.size());
	if (hasLongNumber)
		tip += QStringLiteral("\n\n��⵽�������ֶΣ������Ƚ���Ԫ���ʽ����Ϊ�ı�");
	QMessageBox::information(this, QStringLiteral("����ΪExcel"), tip);
}

/**
 * �ֶ��Ҽ��˵�
 */
void ListAutoSort::slotFieldItemMenu(QPoint p)
{
	QListWidgetItem* item = ui.fieldsList->itemAt(p);
	if (item == NULL)
		return;

	QMenu* menu = new QMenu(this);
	QAction* renameAction = new QAction(QStringLiteral("������"), this);
	QAction* deleteAction = new QAction(QStringLiteral("ɾ��"), this);
	QAction* moveUpAction = new QAction(QStringLiteral("����"), this);
	QAction* moveDownAction = new QAction(QStringLiteral("����"), this);
	QAction* moveTopAction = new QAction(QStringLiteral("�ö�"), this);
	QAction* moveBottomAction = new QAction(QStringLiteral("�õ�"), this);
	menu->addAction(renameAction);
	menu->addAction(deleteAction);
	menu->addAction(moveUpAction);
	menu->addAction(moveDownAction);
	menu->addAction(moveTopAction);
	menu->addAction(moveBottomAction);
	connect(renameAction, SIGNAL(triggered()), this, SLOT(slotFieldItemRename()));
	connect(deleteAction, SIGNAL(triggered()), this, SLOT(slotFieldItemDelete()));
	connect(moveUpAction, SIGNAL(triggered()), this, SLOT(slotFieldItemMoveUp()));
	connect(moveDownAction, SIGNAL(triggered()), this, SLOT(slotFieldItemMoveDown()));
	connect(moveTopAction, SIGNAL(triggered()), this, SLOT(slotFieldItemMoveTop()));
	connect(moveBottomAction, SIGNAL(triggered()), this, SLOT(slotFieldItemMoveBottom()));

	menu->exec(QCursor::pos());
}

void ListAutoSort::slotFieldItemRename()
{
	QListWidgetItem* item = ui.fieldsList->currentItem();
	if (item == NULL)
		return;
	int index = ui.fieldsList->currentIndex().row();
	if (index < 0 || index >= fields.size())
		return;

	ui.fieldsList->editItem(item);
}

/**
 * ɾ���ֶ�
 */
void ListAutoSort::slotFieldItemDelete()
{
	QListWidgetItem* item = ui.fieldsList->currentItem();
	if (item == NULL)
		return;
	int index = ui.fieldsList->currentIndex().row();
	if (index < 0 || index >= fields.size())
		return;

	// ɾ���ֶ��б�
	fields.removeAt(index);

	ui.fieldsList->takeItem(index); // ��֪��ΪʲôremoveWidgetItem��Ч
	delete item; // takeItem ��Ҫ�ֶ� delete

	ui.tableWidget->removeColumn(index);

	writeFieldsInfo();
}

void ListAutoSort::slotFieldItemMoveUp()
{
	QListWidgetItem* item = ui.fieldsList->currentItem();
	if (item == NULL)
		return;
	int index = ui.fieldsList->currentIndex().row();
	if (index < 0 || index >= fields.size())
		return;

	if (index == 0)
		return;

	moveListItem(index, index - 1);
}

void ListAutoSort::slotFieldItemMoveDown()
{
	QListWidgetItem* item = ui.fieldsList->currentItem();
	if (item == NULL)
		return;
	int index = ui.fieldsList->currentIndex().row();
	if (index < 0 || index >= fields.size())
		return;

	if (index >= fields.size()-1)
		return;

	moveListItem(index, index + 1);
}

void ListAutoSort::slotFieldItemMoveTop()
{
	QListWidgetItem* item = ui.fieldsList->currentItem();
	if (item == NULL)
		return;
	int index = ui.fieldsList->currentIndex().row();
	if (index < 0 || index >= fields.size())
		return;

	if (index == 0)
		return;

	moveListItem(index, 0);
}

void ListAutoSort::slotFieldItemMoveBottom()
{
	QListWidgetItem* item = ui.fieldsList->currentItem();
	if (item == NULL)
		return;
	int index = ui.fieldsList->currentIndex().row();
	if (index < 0 || index >= fields.size())
		return;

	if (index >= fields.size()-1)
		return;

	moveListItem(index, fields.size() - 1);
}

/**
 * �ֶ��б�λ�øı�
 */
void ListAutoSort::slotFieldItemRowChanged()
{
	int index = ui.fieldsList->currentIndex().row();
	if (index < 0 || index >= fields.size())
		return;
	QString pat = fields[index].getPattern();
	ui.patternEdit->setText(pat);
}

/**
 * ˫���޸��ı�����
 */
void ListAutoSort::slotFieldItemTextModified(int row, QString text)
{
	if (row < 0 || row >= fields.size())
		return;
	if (fields[row].getName() == text)
		return;

	fields[row].setName(text);
	ui.fieldsList->setCurrentRow(row);

	if (fields[row].getPattern().isEmpty())
	{
		QString def_reg = getDefaultRegex(text);
		fields[row].setPattern(def_reg);
		if (ui.fieldsList->currentIndex().row() == row)
			ui.patternEdit->setText(def_reg);
	}

	//setTableHeader();
	QTableWidgetItem* item = ui.tableWidget->item(0, row);
	if (item == NULL) // ʵ����NULL����
	{
		setTableHeader();
	}
	else
	{
		item->setText(text);
		ui.tableWidget->setHorizontalHeaderItem(row, item);
	}

	writeFieldsInfo();
}

/**
 * �޸�������ʽ�¼�
 */
void ListAutoSort::slotFieldItemPatternModified(QString text)
{
	int index = ui.fieldsList->currentIndex().row();
	if (index < 0 || index >= fields.size())
		return;
	fields[index].setPattern(text);
	writeFieldsInfo();
}

/**
 * ��ȡ�ض��ֶ�����������ʽ
 */
QString ListAutoSort::getDefaultRegex(QString field)
{
	if (field == QStringLiteral("����"))
		return "^[^\\x00-\\xff]{2,4}$";
	if (field == QStringLiteral("�Ա�"))
		return "[01��Ů]";
	if (field == QStringLiteral("����"))
		return "^\\d{1,2}$";
	if (field == QStringLiteral("�༶"))
		return "^[^\\x00-\\xff]{2,15}\\d{2,4}�༶?$";
	if (field == QStringLiteral("�ֻ�") || field == QStringLiteral("�ֻ���"))
		return "^1\\d{10}$";
	if (field == QStringLiteral("����"))
		return "^[A-Za-z\\d]+([-_.][A-Za-z\\d]+)*@([A-Za-z\\d]+[-.])+[A-Za-z\\d]{2,4}$";
	if (field == QStringLiteral("ѧ��"))
		return "^20\\d{10,11}$";
	if (field == QStringLiteral("���֤"))
		return "^\\d{17}.$";
	if (field == QStringLiteral("רҵ"))
		return "^[^\\x00-\\xff]{2,15}$";
	if (field == QStringLiteral("ѧУ") || field == QStringLiteral("��ѧ"))
		return "^[^\\x00-\\xff]{2,10}(��ѧ|ѧУ|ѧԺ)$";
	if (field == QStringLiteral("�꼶"))
		return "(Сѧ|����?|����?|��ѧ?|��)(һ������)(�꼶)?";

	return "";
}

