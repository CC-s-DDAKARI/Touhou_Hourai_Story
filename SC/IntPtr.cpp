using namespace SC;

IntPtr::IntPtr() : IntPtr( nullptr )
{

}

IntPtr::IntPtr( void* ptr ) : ValueType()
	, Value( ptr )
{

}

IntPtr::IntPtr( const IntPtr& copy ) : IntPtr( copy.Value )
{

}

String IntPtr::ToString()
{
	std::wostringstream woss;
	if ( Value )
	{
		woss << L"SC.IntPtr(" << std::hex << std::showbase << std::setw( sizeof( void* ) * 2 ) << std::setfill( L'0' ) << Value << L")";
		return woss.str().c_str();
	}
	else
	{
		return L"SC.IntPtr(nullptr)";
	}
}

bool IntPtr::Equals( object obj )
{
	if ( IntPtr v; obj.Is<IntPtr>( &v ) )
	{
		return Equals( v.Value );
	}
	else
	{
		return obj.Get();
	}
}

bool IntPtr::Equals( void* right )
{
	return Value == right;
}

int IntPtr::CompareTo( void* right )
{
	if ( Value < right ) return -1;
	else if ( Value > right ) return 1;
	else return 0;
}

IntPtr::operator void* ( )
{
	return Value;
}

IntPtr& IntPtr::operator=( const IntPtr& copy )
{
	Value = copy.Value;
	return *this;
}

bool IntPtr::operator==( const IntPtr& right ) const
{
	return Value == right.Value;
}

bool IntPtr::operator!=( const IntPtr& right ) const
{
	return Value != right.Value;
}

bool IntPtr::operator< ( const IntPtr& right ) const
{
	return Value <  right.Value;
}

bool IntPtr::operator> ( const IntPtr& right ) const
{
	return Value >  right.Value;
}

bool IntPtr::operator<=( const IntPtr& right ) const
{
	return Value <= right.Value;
}

bool IntPtr::operator>=( const IntPtr& right ) const
{
	return Value >= right.Value;
}