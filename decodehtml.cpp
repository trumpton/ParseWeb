//
// decodehtml.cpp
//
//

#include <QString>
#include <QRegularExpression>

// Generate a Unicode Character in a String from an
// unsigned long int
QString unicodeFromUnsignedLong(unsigned long int i) ;


QString decodehtml(QString src)
{

  QString result ;
  result = src ;

  const static QString  decodehtmldata[] = {
  "&euro;", "€", "&quot;", "\"", "&amp;", "&", "&bull;", "•",
  "&lt;", "<", "&gt;", ">", "&nbsp;", " ", "&iexcl;", "¡",
  "&cent;", "¢", "&pound;", "£", "&curren;", "¤", "&yen;", "¥",
  "&brvbar;", "¦", "&sect;", "§", "&uml;", "¨", "&copy;", "©",
  "&ordf;", "ª", "&not;", "¬", "&shy;", "­", "&reg;", "®",
  "&macr;", "¯", "&deg;", "°", "&plusmn;", "±", "&sup2;", "²",
  "&sup3;", "³", "&acute;", "´", "&micro;", "µ", "&para;", "¶",
  "&middot;", "·", "&cedil;", "¸", "&sup1;", "¹", "&ordm;", "º",
  "&raquo;", "»", "&frac14;", "¼", "&frac12;", "½", "&frac34;", "¾",
  "&iquest;", "¿", "&Agrave;", "À", "&Aacute;", "Á", "&Acirc;", "Â",
  "&Atilde;", "Ã", "&Auml;", "Ä", "&Aring;", "Å", "&AElig;", "Æ",
  "&Ccedil;", "Ç", "&Egrave;", "È", "&Eacute;", "É", "&Ecirc;", "Ê",
  "&Euml;", "Ë", "&Igrave;", "Ì", "&Iacute;", "Í", "&Icirc;", "Î",
  "&Iuml;", "Ï", "&ETH;", "Ð", "&Ntilde;", "Ñ", "&Ograve;", "Ò",
  "&Oacute;", "Ó", "&Ocirc;", "Ô", "&Otilde;", "Õ", "&Ouml;", "Ö",
  "&times;", "×", "&Oslash;", "Ø", "&Ugrave;", "Ù", "&Uacute;", "Ú",
  "&Ucirc;", "Û", "&Uuml;", "Ü", "&Yacute;", "Ý", "&THORN;", "Þ",
  "&szlig;", "ß", "&agrave;", "à", "&aacute;", "á", "&acirc;", "â",
  "&atilde;", "ã", "&auml;", "ä", "&aring;", "å", "&aelig;", "æ",
  "&ccedil;", "ç", "&egrave;", "è", "&eacute;", "é", "&ecirc;", "ê",
  "&euml;", "ë", "&igrave;", "ì", "&iacute;", "í", "&icirc;", "î",
  "&iuml;", "ï", "&eth;", "ð", "&ntilde;", "ñ", "&ograve;", "ò",
  "&oacute;", "ó", "&ocirc;", "ô", "&otilde;", "õ", "&ouml;", "ö",
  "&divide;", "÷", "&oslash;", "ø", "&ugrave;", "ù", "&uacute;", "ú",
  "&ucirc;", "û", "&uuml;", "ü", "&yacute;", "ý", "&thorn;", "þ",
  "", "" } ;


  //
  // Replace entries in table
  //

  for (int x=0; !decodehtmldata[x].isEmpty(); x+=2) {
          result.replace(decodehtmldata[x], decodehtmldata[x+1]) ;
  }

  //
  // Replace all Hex Escape Sequences
  //

  bool hasMatch = false ;
  bool hasPartialMatch = false ;
  QString search = "&#x([0-9a-fA-D]+);" ;

  do {

      static QRegularExpression re ;
      re.setPattern(search) ;
      re.setPatternOptions(QRegularExpression::MultilineOption|QRegularExpression::InvertedGreedinessOption) ;

      QRegularExpressionMatch rem ;
      rem = re.match(result) ;
      hasMatch = rem.hasMatch();
      hasPartialMatch = rem.hasPartialMatch();

      if (hasMatch || hasPartialMatch) {
          QStringList captured = rem.capturedTexts() ;
          if (captured.length()>=2) {
              QString number = captured.at(1) ;
              QString escape = captured.at(0) ;
              bool success = false ;
              unsigned long int ii = number.toULong(&success, 16) ;
              QString unicode = unicodeFromUnsignedLong(ii) ;
              result.replace(escape, unicode) ;
          }
      }

  } while (hasMatch || hasPartialMatch) ;


  //
  // Replace all decimal escape sequences
  //

  search = "&#([0-9]+);" ;

  do {

      static QRegularExpression re ;
      re.setPattern(search) ;
      re.setPatternOptions(QRegularExpression::MultilineOption|QRegularExpression::InvertedGreedinessOption) ;

      QRegularExpressionMatch rem ;
      rem = re.match(result) ;
      hasMatch = rem.hasMatch();
      hasPartialMatch = rem.hasPartialMatch();

      if (hasMatch || hasPartialMatch) {
          QStringList captured = rem.capturedTexts() ;
          if (captured.length()>=2) {
              QString number = captured.at(1) ;
              QString escape = captured.at(0) ;
              bool success = false ;
              unsigned long int ii = number.toULong(&success, 10) ;
              QString unicode = unicodeFromUnsignedLong(ii) ;
              result.replace(escape, unicode) ;
          }
      }

  } while (hasMatch || hasPartialMatch) ;

  return result ;
}

QString unicodeFromUnsignedLong(unsigned long int i)
{
    QString result = "?" ;
    if (i<=65535) {
        // QChar can handle unicode values up to 65535
        result = QChar((unsigned int)i) ;
    }
    // TODO: Build other lengths of unicode in multiple QChars
    return result ;
}
