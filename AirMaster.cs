using System.IO.Ports;

using (var serial = new SerialPort("COM7", 19200, Parity.None, 8, StopBits.One))
{
    serial.Open();

    while (serial.IsOpen)
    {
        Thread.Sleep(2000);

        serial.Write(new byte[] { 0x55, 0xCD, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x69, 0x0D, 0x0A }, 0, 13);

        var buffer = new byte[40];
        serial.Read(buffer, 0, 40);

        var pm25 = buffer[2] | buffer[1] << 8;
        var pm10 = buffer[4] | buffer[3] << 8;
        var hcho = buffer[6] | buffer[5] << 8;
        var tvoc = buffer[8] | buffer[7] << 8;
        var co2 = buffer[10] | buffer[9] << 8;
        double temp = buffer[12] | buffer[11] << 8;
        double humidity = buffer[14] | buffer[13] << 8;

        var ppm03 = buffer[20] | buffer[19] << 8;
        var ppm05 = buffer[22] | buffer[21] << 8;
        var ppm1 = buffer[24] | buffer[23] << 8;
        var ppm2 = buffer[26] | buffer[25] << 8;
        var ppm5 = buffer[28] | buffer[27] << 8;
        var ppm10 = buffer[30] | buffer[29] << 8;

        var check = buffer[37] | buffer[36] << 8;
        var sum = buffer.Take(36).Sum(x => x);

        if (check == sum && check != 0)
        {
            Console.Clear();

            Console.WriteLine($"PM2.5: {pm25} ug/m3");
            Console.WriteLine($"PM10: {pm10} ug/m3");
            Console.WriteLine($"HCHO: {hcho} ug/m3");
            Console.WriteLine($"TVOC: {tvoc} ug/m3");
            Console.WriteLine($"CO2: {co2} ppm");
            Console.WriteLine($"T: {temp / 100} °C");
            Console.WriteLine($"H: {humidity / 100} %");

            Console.WriteLine($"PM0.3: {ppm03} ppm");
            Console.WriteLine($"PM0.5: {ppm05} ppm");
            Console.WriteLine($"PM1.0: {ppm1} ppm");
            Console.WriteLine($"PM2.5: {ppm2} ppm");
            Console.WriteLine($"PM5.0: {ppm5} ppm");
            Console.WriteLine($"PM10: {ppm10} ppm");
        }
        else
        {
            Console.WriteLine($"Reading data...");
        }
    }
}

Console.ReadLine();