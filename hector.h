#ifndef _HECTOR_H
#define _HECTOR_H

/*
 * The next line is used because Codewarrior has a conflict with
 * the STL hector. Make sure to put the #include of this file
 * AFTER all the system includes.
 */
#define hector hector
#include <cstddef> // for NULL

class ArrayIndexOutOfBounds { };

template <class Object>
class hector
{
  public:
    explicit hector( int theSize = 0 ) : currentSize( theSize )
      { objects = new Object[ currentSize ]; }
    hector( const hector & rhs ) : objects( NULL )
      { operator=( rhs ); }
    ~hector( )
      { delete [ ] objects; }

    int size( ) const
      { return currentSize; }

    Object & operator[]( int index )
    {
                                                     #ifndef NO_CHECK
        if( index < 0 || index >= currentSize )
            throw ArrayIndexOutOfBounds( );
                                                     #endif
        return objects[ index ];
    }

    const Object & operator[]( int index ) const
    {
                                                     #ifndef NO_CHECK
        if( index < 0 || index >= currentSize )
            throw ArrayIndexOutOfBounds( );
                                                     #endif
        return objects[ index ];
    }


    const hector & operator = ( const hector & rhs );
    void resize( int newSize );
  private:
    int currentSize;
    Object * objects;
};

#include "hector.cpp"
#endif
