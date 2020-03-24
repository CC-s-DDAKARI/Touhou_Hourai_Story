using namespace SC;
using namespace SC::Game;

Sphere::Sphere()
{

}

Sphere::Sphere( Vector3 center, double radius ) : Sphere()
{
	this->Center = center;
	this->Radius = radius;
}

Sphere::Sphere( const Sphere& copy ) : Sphere( copy.Center, copy.Radius )
{

}

bool Sphere::operator==( const Sphere& right ) const
{
	return Center == right.Center
		&& Radius == right.Radius;
}

bool Sphere::operator!=( const Sphere& right ) const
{
	return Center != right.Center
		|| Radius != right.Radius;
}