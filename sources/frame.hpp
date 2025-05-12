#pragma once

template <typename T>
class frame
{
    private:
        T* data;
    public:   
        const unsigned width;
        const unsigned height;             
        constexpr void set(const unsigned&, const unsigned&, const T&) noexcept;
        constexpr T    get(const unsigned&, const unsigned&) const noexcept;
        constexpr T*   ref(const unsigned&, const unsigned&) const noexcept;
        constexpr T*   ref() const noexcept { return data; }
        constexpr void clr(const T&) noexcept;
        constexpr frame(const unsigned&, const unsigned&) noexcept;
       ~frame();
};



template <typename T>
constexpr void frame<T>::set(const unsigned& x, const unsigned& y, const T& value) noexcept
{
    if((x < width) && (y < height))
    data[x + y * width] = value;
}

template <typename T>
constexpr T frame<T>::get(const unsigned& x, const unsigned& y) const noexcept
{
    if((x < width) && (y < height)) return data[x + y * width];
    return data[0];
}

template <typename T>
constexpr T* frame<T>::ref(const unsigned& x, const unsigned& y) const noexcept
{
    if((x < width) && (y < height)) return &data[x + y * width];
    return &data[0];
}

template <typename T>
constexpr void frame<T>::clr(const T& value) noexcept
{
    for(unsigned i = 0; i < (height * width); ++i) data[i] = value;
}

template <typename T>
constexpr frame<T>::frame(const unsigned& x, const unsigned& y) noexcept: width(x), height(y)
{
    data = new T[height * width];
}

template <typename T>
inline frame<T>::~frame()
{
    delete[] data;
}




void line(frame<unsigned>* canvas, int xa, int ya, int xb, int yb, unsigned color) {
    int dx = abs(xb - xa);
    int dy = abs(yb - ya);
    int sx = (xa < xb) ? 1 : -1;
    int sy = (ya < yb) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        canvas->set(xa, ya, color); // Set the pixel

        if (xa == xb && ya == yb) break; // End condition

        int e2 = err * 2;
        if (e2 > -dy) {
            err -= dy;
            xa += sx;
        }
        if (e2 < dx) {
            err += dx;
            ya += sy;
        }
    }
}

void draw_rectangle(frame<unsigned>* canvas, int xo, int yo, int width, int height, const unsigned alpha)
{
    int xe = xo + width;
    int ye = yo + height;
    
    for(int i = xo; i <= xe; i++)
    {
        canvas->set(i, yo, alpha);
        canvas->set(i, ye, alpha);
    }
    for(int i = yo; i <= ye; i++)
    {
        canvas->set(xo, i, alpha);
        canvas->set(xe, i, alpha);
    }
}

void plot_function(frame<unsigned>* canvas, int width, int height, float (*func)(float), float xmin, float xmax, unsigned color) 
{
    float xscale = width / (xmax - xmin);
    float yscale = height / 2.0f; // Normalize so 0 is the middle

    int prev_x = 0, prev_y = 0;
    bool first_point = true;

    for (int i = 0; i < width; i++) 
    {
        float x = xmin + (i / xscale);
        float y = func(x); // Get function output
        int screen_x = i;
        int screen_y = (int)(height / 2.0f - y * yscale); // Flip for screen coordinates

        canvas->set(screen_x, screen_y, color);

        if (!first_point) 
        {
            line(canvas, prev_x, prev_y, screen_x, screen_y, color);
        }

        prev_x = screen_x;
        prev_y = screen_y;
        first_point = false;
    }
}
