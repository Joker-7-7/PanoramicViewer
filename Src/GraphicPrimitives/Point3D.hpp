#ifndef POINT3D_HPP
#define POINT3D_HPP

namespace GraphicPrimitives
{
    /**
     * 3D point including three coordinates: x, y, z
     * <p>
	 * Used to work with geometry in 3D space
	 * <p>
     *
  	 * Usage example:
     * @code
     * double coordinates[3] = { 1.25, 2.33, 3.94 };
     * Point3D point(coordinates);
     * point.round();
     * @endcode
     *
     * @author Ggevorgyan 
     * @since  0.1.0
     */
	class Point3D final
	{
	public:
        /**
         * X coordinate of a point in space
         */
		double x;
        /**
        * Y coordinate of a point in space
        */
		double y;
        /**
        * Z coordinate of a point in space
        */
		double z;

        /**
         * Constructs the point with X, Y, Z coordinates equal to <code>0.0</code>
         */
		Point3D();

        /**
         * Constructs the point with given coordinates X, Y, Z.
         * 
         * @param x X coordinate
         * @param y Y coordinate
         * @param z Z coordinate
         */
		Point3D(double x, double y, double z);

        /**
         * Constructs the point from 3-element array [x, y, z].
         * 
         * @param value pointer to an array
         * 
         * @attention   passing pointer to invalid or forbidden memory cause undefined behaviour
         */
		explicit Point3D(const double* value);

        /**
         * Copy constructor.
         * 
         * @param point point to be copied
         */
		Point3D(const Point3D& point) = default;

        /**
         * Move constructor.
         * 
         * @param point point to be moved
         */
		Point3D(Point3D&& point) noexcept = default;

       /**
         * Copy assignment operator.
         * 
         * @param  value point to be copied
         * @return       reference to the current instance
         */
		Point3D& operator=(const Point3D& value) = default;

        /**
         * Move assignment operator.
         * 
         * @param  value point to be moved
         * @return       reference to the current instance
         */
		Point3D& operator=(Point3D&& value) noexcept = default;

        /**
         * Assignment operator.
         * 
         * @param  value pointer to a 3-element array with [x, y, z] coordinates.
         * @return       reference to the current instance
         * 
         * @attention    passing pointer to invalid or forbidden memory cause undefined behaviour
         */
		Point3D& operator= (const double* value);

        /**
         * Rounds all coordinates to the nearest integer values
         */
        void round() noexcept;
	};
}

#endif // POINT3D_HPP
