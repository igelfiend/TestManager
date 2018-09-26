#ifndef UTILS_H
#define UTILS_H

#include <QDomElement>
#include <QList>
#include <QVariant>
#include <QVector>
#include <QLayout>

#define FIELDS QVector< QPair< QDomNode, QTextEdit* > >

enum StringType
{
	Array,
	BracketsArray,
	BracketsMatrix,
	Matrix,
	Error,
	None
};

class Param;
class Manager;
class SpoilerGroupBox;
class QGroupBox;
class QTextEdit;

class Utils
{
public:
	Utils();

	static	QDomElement getXmlElement(	const QDomNode & domNode,
										const QString& path,
										Qt::CaseSensitivity caseSensitivity );

    static	void	printNode(const QDomNode &node );
    static	QVector< QVector< QString > > parseString(QString str);
    static	QString saveToString( const QVector<QVector<QString>> &matrix, StringType string_type );
    static	QString saveToString( const QVector<QString> &matrix, StringType string_type );
    static	StringType getStringType(QString str);
	static	QString spacesToTabs(QString str);
	static	void	clearLayout( QLayout *layout );
    static QDomText TextInside( const QDomNode &node );
    static void		removeChilds(QDomNode node );

	static	void	fixAccuracyTestRange(const QString &test_name, const QString &version, Manager *manager );

	static	void	addEquipToPerformance(Manager *manager );
    static  void    fixEquipOrderInPeriodic( Manager *manager );

    static  void    fixPowerAccuracyNaming( Manager *manager );
    static  void    addTagToMain(Manager *manager , const QString &tag_name, const QString &paste_after = QString() );

    static  void    addTestToConfigs( const QString &test_code, bool fInsertFirst, const QString &prevTest, Manager *manager );

	static	bool	NodeIsFieldType( const QDomNode &node );
	static	SpoilerGroupBox	*NodeToGroupBox(const QDomNode &node, FIELDS &fields);
    static  bool    ReplaceInFile(const QString &src_filename, const QString &tgt_filename, const QMap< QString, QString > replace_map);
    static  bool    ReplaceColorInSvgs(const QString &dev_dir_path, const QStringList &devices, const QMap<QString, QString> &replace_map, bool fReplaceOrigin, QWidget *parent );

    //!
    //! \brief FixSvgStyle Process all svgs
    //! \param img_foulder Name pics foulder
    //! \param pic_size Area of pic_block
    //! \param style Stylename
    //! \param css_string Css for settings Stylename in CDATA block
    //! \return True if success
    //!
    static  bool    FixSvgStyle( const QString &img_foulder, QPoint pic_size, const QString &style, const QString &css_string, QWidget *parent );

    //!
    //! \brief FixSvgStyleInFile Process svg file. Fill specific square block with size "pic_size" with "style" and "css_string"
    //! \param filepath Path to file
    //! \param pic_size Area of pic_block
    //! \param style Stylename
    //! \param css_string Css for settings Stylename in CDATA block
    //! \return True if success
    //!
    static  bool    FixSvgStyleInFile( const QString &filepath, QPoint pic_size, const QString &style, const QString &css_string );

private:
	static void		insertNodeAndAddToList( QDomNode &target, QStringList &target_list, const QString &node_name, const QString &data );
	static void		insertNode( QDomNode &target, const QString &node_name, const QString &data );
	static QDomNode	createEquipItem(const QStringList &dev_list, QDomDocument &doc );
	static void		addElementsToList( QStringList &target_list, const QString &data );
    static QVector< QString > StringToVector(const QString &str);

    static QPoint getWidthAndHeightFromPointsParameterInSvg( const QString &points_str );
    static bool   processRowInSvg(QString &row, QPoint pic_size, const QString &stylename, const QString &css_style);
};

#endif // UTILS_H

