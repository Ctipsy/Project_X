#ifndef ADMIN_INDEX_CONF_H
#define ADMIN_INDEX_CONF_H

#include "index/index.h"
#include <QDialog>
#include"svg_show/svgwindow.h"  //8.22添加


QList<Index *> index_select_return();

namespace Ui {
class admin_index_conf;
}

class admin_index_conf : public QDialog
{
    Q_OBJECT

public:
    explicit admin_index_conf(QWidget *parent = 0);
    ~admin_index_conf();

private slots:
    void on_add_root_index_clicked();
    void on_add_child_index_clicked();
    void on_modifyIndex_clicked();
    void on_deletindex_clicked();
    void on_inputIndex_textEdited(const QString &arg1);
    void on_index_select_currentIndexChanged(int index);
    QTreeWidgetItem * AddTreeRoot(QString name, QString desc);
    QTreeWidgetItem * AddTreeNode(QTreeWidgetItem *parent, QString name, QString desc);


    void on_delete_access_clicked();

    void on_index_write_access_clicked();

    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void on_treeWidget_modify_itemChanged(QTreeWidgetItem *item, int column);

    void on_treeWidget_modify_itemClicked(QTreeWidgetItem *item, int column);


    void on_pushButton_clicked();
    void show_modify_success();

    void on_svg_view_clicked(bool checked);

private:
    Ui::admin_index_conf *ui;
	bool m_bEnterContent;
    QString m_strEditBefore;
    QString m_strEditAfter;
    QString m_strNumEditBefore;
    QString m_strNumEditAfter;
    QString m_strFather;
    QString m_strChild;
    QTreeWidgetItem *lastOpen;
    //添加数据库的相关操作	
    Index *index_conf;
    QList<Index*> index_select;
    SvgWindow *svgWindow;  //8.6添加

};

#endif // ADMIN_INDEX_CONF_H
