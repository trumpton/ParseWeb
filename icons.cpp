#include "icons.h"

const char *icons[] = { "parseweb", "network", "search", "news", "rss", "television", "france",
			"germany", "italy", "spain", "uk", NULL } ;

int getIconNumber(QString name)
{
	name = name.toLower() ;
	for (int i=0; icons[i]!=NULL; i++)
		if (name.compare(icons[i])==0) return i ;
	return 0 ;
}

QString validIcons()
{
	QString iconslist ;
	for (int i=0; icons[i]!=NULL; i++)
		iconslist = iconslist + QString(icons[i]) + QString(" ") ;
	return iconslist ;
}


