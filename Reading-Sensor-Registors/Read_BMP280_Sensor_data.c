/*
 * bmp280_spi_driver.c
 *
 * Simple SPI driver for BMP280 sensor
 * Includes CONFIG and CTRL_MEAS register handling, and raw pressure/temperature reading
 *
 * Author: You
 * License: GPL
 *
 * This driver demonstrates:
 * 1. SPI device initialization
 * 2. Register read/write (CHIP_ID, CONFIG, CTRL_MEAS)
 * 3. Reading raw pressure and temperature data
 *
 * All register addresses and operation values use macros.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/spi/spi.h>
#include <linux/err.h>

/* ---------- Driver Definitions ---------- */
#define DRIVER_NAME        "spi_bmp280"
#define BMP280_BUS_NUM     0       // SPI bus number (SPI0)
#define BMP280_CS          0       // Chip select 0

/* ---------- SPI Operation Macros ---------- */
#define BMP280_SPI_READ    0x80    // MSB=1 for read operation
#define BMP280_SPI_WRITE   0x7F    // MSB=0 for write operation

/* ---------- BMP280 Register Addresses ---------- */
#define BMP280_REG_CHIP_ID      0xD0
#define BMP280_REG_CTRL_MEAS    0xF4
#define BMP280_REG_CONFIG       0xF5
#define BMP280_REG_PRESS_MSB    0xF7   // Pressure MSB
#define BMP280_REG_PRESS_LSB    0xF8
#define BMP280_REG_PRESS_XLSB   0xF9
#define BMP280_REG_TEMP_MSB     0xFA   // Temperature MSB
#define BMP280_REG_TEMP_LSB     0xFB
#define BMP280_REG_TEMP_XLSB    0xFC

/* ---------- Example Configuration Macros ---------- */
#define BMP280_CONFIG_VAL       0x40   // CONFIG register value: standby=0.5ms, filter off
#define BMP280_CTRL_MEAS_VAL    0x27   // CTRL_MEAS: temp x1, press x1, normal mode

/* ---------- Global SPI device pointer ---------- */
static struct spi_device *bmp280_device;

/* ---------- Module Initialization Function ---------- */
/*
 * bmp280_init
 * ----------------
 * This function is called when the kernel module is loaded.
 * Its responsibilities:
 * 1. Locate the SPI master controller that will manage communication with the sensor.
 * 2. Create and register the BMP280 SPI device with the kernel.
 * 3. Configure the SPI device (mode, speed, bits per word).
 * 4. Perform example read/write operations for CHIP_ID, CONFIG, CTRL_MEAS registers.
 * 5. Read raw pressure and temperature data from the sensor.
 *
 * Returns:
 * 0 on success, negative error code on failure.
 */
static int __init bmp280_init(void)
{
    struct spi_master *master;
    struct spi_board_info spi_device_info = {
        .modalias = DRIVER_NAME,
        .max_speed_hz = 1000000,
        .bus_num = BMP280_BUS_NUM,
        .chip_select = BMP280_CS,
        .mode = SPI_MODE_3,
    };

    u8 tx[2], rx[6];
    int ret;
    u8 chip_id, reg_val;
    u32 raw_press, raw_temp;

    pr_info(DRIVER_NAME ": init\n");

    /* ---------- 1. Get SPI Master ---------- */
    /*
     * spi_busnum_to_master(bus_num)
     * - Finds the SPI master controller registered for the given bus.
     * - Necessary to know which hardware SPI controller will act as master.
     * - Returns pointer to spi_master structure, or NULL if not found.
     */
    master = spi_busnum_to_master(BMP280_BUS_NUM);
    if (!master) {
        pr_err(DRIVER_NAME ": SPI bus %d not found\n", BMP280_BUS_NUM);
        return -ENODEV;
    }

    /* ---------- 2. Create SPI Device ---------- */
    /*
     * spi_new_device(master, &spi_device_info)
     * - Registers a new SPI slave device (BMP280) with the SPI master.
     * - Returns a pointer to the SPI device structure.
     * put_device(&master->dev) decrements the reference count on master after creating device.
     * This is necessary to avoid memory leaks.
     */
    bmp280_device = spi_new_device(master, &spi_device_info);
    put_device(&master->dev);
    if (!bmp280_device) {
        pr_err(DRIVER_NAME ": failed to create SPI device\n");
        return -ENODEV;
    }

    /* ---------- 3. Configure SPI Device ---------- */
    /*
     * Sets bits per word and calls spi_setup() to configure the SPI hardware.
     * spi_setup() ensures SPI mode, speed, and word length are applied.
     * Necessary before any SPI communication occurs.
     */
    bmp280_device->bits_per_word = 8;
    ret = spi_setup(bmp280_device);
    if (ret) {
        pr_err(DRIVER_NAME ": spi_setup failed\n");
        spi_unregister_device(bmp280_device);
        return ret;
    }

    /* ---------- 4. Read CHIP_ID ---------- */
    /*
     * Reads BMP280 chip ID (0xD0). MSB=1 indicates a read operation.
     * Necessary to verify sensor presence and correct communication.
     */
    tx[0] = BMP280_REG_CHIP_ID | BMP280_SPI_READ;
    ret = spi_write_then_read(bmp280_device, tx, 1, &chip_id, 1);
    if (ret) {
        pr_err(DRIVER_NAME ": failed to read CHIP_ID\n");
    } else {
        pr_info(DRIVER_NAME ": CHIP_ID = 0x%X (should be 0x58)\n", chip_id);
    }

    /* ---------- 5. Write CONFIG Register ---------- */
    /*
     * Writes example configuration value to CONFIG register.
     * MSB=0 indicates a write operation.
     * Necessary to setup standby time and filter options.
     */
    tx[0] = BMP280_REG_CONFIG & BMP280_SPI_WRITE;
    tx[1] = BMP280_CONFIG_VAL;
    ret = spi_write(bmp280_device, tx, 2);
    if (ret) {
        pr_err(DRIVER_NAME ": failed to write CONFIG register\n");
    }

    /* ---------- 6. Read CONFIG Register Back ---------- */
    /*
     * Verifies that CONFIG register was correctly written.
     * Optional, but useful for debugging.
     */
    tx[0] = BMP280_REG_CONFIG | BMP280_SPI_READ;
    ret = spi_write_then_read(bmp280_device, tx, 1, &reg_val, 1);
    if (ret) {
        pr_err(DRIVER_NAME ": failed to read CONFIG register\n");
    } else {
        pr_info(DRIVER_NAME ": CONFIG register = 0x%X\n", reg_val);
    }

    /* ---------- 7. Write CTRL_MEASUREMENT Register ---------- */
    /*
     * Writes CTRL_MEAS register to configure oversampling and normal mode.
     * Necessary to start sensor measurements for temperature and pressure.
     */
    tx[0] = BMP280_REG_CTRL_MEAS & BMP280_SPI_WRITE;
    tx[1] = BMP280_CTRL_MEAS_VAL;
    ret = spi_write(bmp280_device, tx, 2);
    if (ret) {
        pr_err(DRIVER_NAME ": failed to write CTRL_MEAS register\n");
    }

    /* ---------- 8. Read CTRL_MEAS Register Back ---------- */
    /*
     * Verifies that CTRL_MEAS register was correctly written.
     * Optional, but ensures communication integrity.
     */
    tx[0] = BMP280_REG_CTRL_MEAS | BMP280_SPI_READ;
    ret = spi_write_then_read(bmp280_device, tx, 1, &reg_val, 1);
    if (ret) {
        pr_err(DRIVER_NAME ": failed to read CTRL_MEAS register\n");
    } else {
        pr_info(DRIVER_NAME ": CTRL_MEAS register = 0x%X\n", reg_val);
    }

    /* ---------- 9. Read Raw Pressure and Temperature (6 bytes F7–FC) ---------- */
    /*
     * Reads 6 bytes of raw data:
     *   - Pressure MSB, LSB, XLSB (20-bit)
     *   - Temperature MSB, LSB, XLSB (20-bit)
     * Necessary to retrieve measurement data from the sensor.
     */
    tx[0] = BMP280_REG_PRESS_MSB | BMP280_SPI_READ;
    ret = spi_write_then_read(bmp280_device, tx, 1, rx, 6);
    if (ret) {
        pr_err(DRIVER_NAME ": failed to read pressure/temperature registers\n");
    } else {
        raw_press = ((u32)rx[0] << 12) | ((u32)rx[1] << 4) | ((rx[2] >> 4) & 0x0F);
        raw_temp  = ((u32)rx[3] << 12) | ((u32)rx[4] << 4) | ((rx[5] >> 4) & 0x0F);

        pr_info(DRIVER_NAME ": Raw Pressure = %u (20-bit)\n", raw_press);
        pr_info(DRIVER_NAME ": Raw Temperature = %u (20-bit)\n", raw_temp);
    }

    return 0;
}

/* ---------- Module Exit Function ---------- */
/*
 * bmp280_exit
 * ----------------
 * Called when the kernel module is removed.
 * Unregisters the SPI device to release resources.
 * Necessary to avoid memory leaks and maintain kernel stability.
 */
static void __exit bmp280_exit(void)
{
    if (bmp280_device) {
        spi_unregister_device(bmp280_device);
        bmp280_device = NULL;
        pr_info(DRIVER_NAME ": SPI device unregistered\n");
    }
    pr_info(DRIVER_NAME ": exit\n");
}

/* ---------- Module Macros ---------- */
module_init(bmp280_init);
