
# define ARROUND_VEC3_ARRAY(point) { \
	{point.x, point.y - 1}, \
	{point.x, point.y + 1}, \
	{point.x - 1, point.y}, \
	{point.x + 1, point.y}, \
	{point.x - 1, point.y - 1}, \
	{point.x - 1, point.y + 1}, \
	{point.x + 1, point.y - 1}, \
	{point.x + 1, point.y + 1}, \
	{point.x, point.y - 2}, \
	{point.x - 1, point.y - 2}, \
	{point.x + 1, point.y - 2}, \
	{point.x - 2, point.y - 2}, \
	{point.x + 2, point.y - 2}, \
	{point.x, point.y + 2}, \
	{point.x - 1, point.y + 2}, \
	{point.x + 1, point.y + 2}, \
	{point.x - 2, point.y + 2}, \
	{point.x + 2, point.y + 2}, \
	{point.x - 2, point.y}, \
	{point.x + 2, point.y}, \
	{point.x - 2, point.y - 1}, \
	{point.x + 2, point.y - 1}, \
	{point.x - 2, point.y + 1}, \
	{point.x + 2, point.y + 1}, \
	{point.x, point.y - 3}, \
	{point.x - 1, point.y - 3}, \
	{point.x + 1, point.y - 3}, \
	{point.x - 2, point.y - 3}, \
	{point.x + 2, point.y - 3}, \
	{point.x - 3, point.y - 3}, \
	{point.x + 3, point.y - 3}, \
	{point.x, point.y + 3}, \
	{point.x - 1, point.y + 3}, \
	{point.x + 1, point.y + 3}, \
	{point.x - 2, point.y + 3}, \
	{point.x + 2, point.y + 3}, \
	{point.x - 3, point.y + 3}, \
	{point.x + 3, point.y + 3}, \
	{point.x - 3, point.y}, \
	{point.x + 3, point.y}, \
	{point.x - 3, point.y - 1}, \
	{point.x + 3, point.y - 1}, \
	{point.x - 3, point.y + 1}, \
	{point.x + 3, point.y + 1}, \
	{point.x - 3, point.y - 2}, \
	{point.x + 3, point.y - 2}, \
	{point.x - 3, point.y + 2}, \
	{point.x + 3, point.y + 2} \
}
