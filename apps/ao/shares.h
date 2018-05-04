// Assign Onward
//
// Shares represents a number of shares in a blockchain's share balance
//
#ifndef SHARES_H
#define SHARES_H

#include <QObject>
#include "bytecodes.h"

class Shares : public QObject
{
    Q_OBJECT
public:
    explicit Shares( __int128 val = 0, QObject *p = nullptr);
             Shares( const Shares &f, QObject *p = nullptr ) : QObject( p ? p : f.parent() ), n( f.n ) {}
             Shares( const QByteArray &ba, QObject *p = nullptr );
        void operator = ( const QByteArray &ba );
        void operator = ( const   Shares &f ) { n = f.n; }
        void operator = ( const __int128 &f ) { n = f;   }
      Shares operator + ( const   Shares &f ) { Shares c(*this); c.n = n + f.n; return c; }
      Shares operator + ( const __int128 &f ) { Shares c(*this); c.n = n + f;   return c; }
      Shares operator - ( const   Shares &f ) { Shares c(*this); c.n = n - f.n; return c; }
      Shares operator - ( const __int128 &f ) { Shares c(*this); c.n = n - f;   return c; }
      Shares operator +=( const   Shares &f ) { n = n + f.n; return *this; }
      Shares operator +=( const __int128 &f ) { n = n + f;   return *this; }
      Shares operator -=( const   Shares &f ) { n = n - f.n; return *this; }
      Shares operator -=( const __int128 &f ) { n = n - f;   return *this; }
        bool operator ==( const   Shares &f ) { return (n == f.n); }
        bool operator ==( const __int128 &f ) { return (n == f  ); }
        bool operator !=( const   Shares &f ) { return (n != f.n); }
        bool operator !=( const __int128 &f ) { return (n != f  ); }
        bool operator <=( const   Shares &f ) { return (n <= f.n); }
        bool operator <=( const __int128 &f ) { return (n <= f  ); }
        bool operator >=( const   Shares &f ) { return (n >= f.n); }
        bool operator >=( const __int128 &f ) { return (n >= f  ); }
        bool operator < ( const   Shares &f ) { return (n <  f.n); }
        bool operator < ( const __int128 &f ) { return (n <  f  ); }
        bool operator > ( const   Shares &f ) { return (n >  f.n); }
        bool operator > ( const __int128 &f ) { return (n >  f  ); }
  QByteArray toByteArray( unsigned char code = AO_TRANSACTION_AMT );

signals:

public slots:

private:
  __int128 n;
};

#endif // SHARES_H
