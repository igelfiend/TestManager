#ifndef MANAGER_H
#define MANAGER_H
#include <QDomElement>
#include <QList>
#include <QWidget>

class QStringList;
class QString;
class Config;
class ConfigInfo;
class MainWindow;
class EditParamForm;

class Manager
{
public:
    explicit Manager(MainWindow *parent);
	~Manager();

	Config *	getConfig( int index ) const;
	int	getConfigsCount() const;

	ConfigInfo *	getConfigInfo() const;
	bool	isLoaded() const;
    void	Load( const QStringList &devices, const QStringList &modes );
	void	Save();
	void	Clear();
	void	ShowLoadedData();

	void setConfigInfo(ConfigInfo *value);

    void setWindow( MainWindow *value );
    void setEditForm( QWidget *form );
	MainWindow	*getWindow() const;

	EditParamForm *getEditForm() const;

	bool	isChanged() const;
	void	setChanged(bool value);

    void	setBarText( const QString &text );

private:
    Q_DISABLE_COPY(Manager)

	bool	loaded;
	bool	changed;
	QString root;
	ConfigInfo	*config_info;
	MainWindow	*window;
	EditParamForm	*edit_form;
	QList<Config *>	configs;
};

#endif // MANAGER_H
