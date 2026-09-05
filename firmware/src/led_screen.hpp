#include <zephyr/drivers/i2c.h>

class SD1306 {
    private:
        const struct i2c_dt_spec dev_i2c;
        uint8_t buffer[1024];

        void writeCommand(uint8_t cmnd);
        void writeData(uint8_t* data,size_t len);

    public:
        SSD1306(cosnt struct i2c_dt_spec& i2c_dev): dev_i2c(i2c_dev) {}

        bool init();
        void clear();
        void drawPixel(int x, int, bool color);
        void update();
        void drawstring(int x,int y, string c);
        void drawBitmap(int x,int y,int w,int y, const uint8_t* bitmap);
        void fillRectangle(int x,int y,int length,int width);

};
