using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Timers;
using System.Diagnostics;
using System.Threading;
using System.Runtime.InteropServices;
using System.Drawing;
using System.Drawing.Imaging;



//using TCPClient;

namespace RoboNightGuard
{

    
    public class SurvillenceCamera

        {
            //public const  byte    COMPRESS_0PIX = 0x00;
            //public const  byte    COMPRESS_2x2PIX = 0x02;
            public const  byte    IMGSIZE_640x480    = 4;
            public const  byte    IMGSIZE_320x240    = 2;
            public const  byte    IMGSIZE_160x120    = 1;
            public const  byte    IMGSCALE_320x240   = 2;
            public const  byte    IMGSCALE_160x120   = 1;
            public const  int     img_width          = 320;
            public const  int     img_height         = 240;
            private const int     WAITING_DELAY      = 10;     // msecond
            public const  int     MAX_CONNECT_RETRY  = 2;
            public const int      MAX_AVAILABLE = 2;
            
            public int            ImgSize            = IMGSIZE_640x480;
            public int            ImgScale           = IMGSCALE_320x240; // should match the size at server
            public int cnt = 0;
        
       
                                                       
            //static public  int    compress_type  = COMPRESS_2x2PIX;
            private  PictureBox    pb;
            private IPAddress      remote;
            private int            port;
            private byte           screen_id;
            private byte           v_id;
            private int            debug_level = 0;
            private TcpClient      client = null;
            private Socket         s_camera = null;
            public SurvillenceCamera()
        {

        }


        void init(PictureBox pb, IPAddress remote, int portn, byte screen_id,byte v_id)
        {
            int len = Form1.elmsize[2*v_id+screen_id] * (ImgScale*img_height/2) * (ImgScale*img_width/2);
            //byte[] data;
            byte [] data = new byte[len];
            DisplayVideoOnScreen(pb, data, remote, port,screen_id, v_id);
        }
        

        public SurvillenceCamera(PictureBox _pb, IPAddress _remote, int _port,byte _screen_id, byte _v_id)
        {
            pb            = _pb;
            remote        = _remote;
            port          = _port;
            screen_id     = _screen_id;
            v_id          = _v_id;
            init(pb, remote, port,screen_id,v_id);
        }
        public void closeSocket( Socket s)
        {
            //Release the socket.
            try
            {
                s.Shutdown(SocketShutdown.Both);
                s.Close();
            }
            catch (Exception e)
            {
                Console.WriteLine("CloseSocket: unable to close socket reason {0}",e.GetType());
            }

        }

        public void closeSocket()
        {
             if (client != null)
            {
                NetworkStream stream = client.GetStream();
                stream.Close();
                client.Close();
            }
            if (s_camera != null)
            {
                s_camera.Close();
            }
        }
        //byte[] is converted to Intptr
        public static IntPtr BytesToIntptr(byte[] bytes)
        {
            int size = bytes.Length;
            IntPtr buffer = Marshal.AllocHGlobal(size);
            try
            {
                Marshal.Copy(bytes, 0, buffer, size);
                return buffer;
            }
            finally
            {
                Marshal.FreeHGlobal(buffer);
            }
        }

       

        void DrawGreyBitmapOptimized(PictureBox form, Byte[] data,byte screen_id)
        {
            //for (int i = 0; i < img_height * img_width / 4; i++)
            //    Console.WriteLine("{0} , {1}", i % (img_width / 2), data[i]);
            //Console.WriteLine("GREY  screenid {0} ", screen_id);
            Stopwatch stopwatch = new Stopwatch();
            stopwatch.Start();
            try
            {
                //for (int i = 0; i < img_height * img_width; i++)
                //    Console.WriteLine("PIX {0} ", finalframe[i]);
                Graphics g;
                g = form.CreateGraphics();
                Bitmap b = new Bitmap(ImgScale*img_width/2, ImgScale*img_height/2, PixelFormat.Format8bppIndexed);
                System.Drawing.Rectangle rect = new System.Drawing.Rectangle(0, 0, ImgScale*img_width/2, ImgScale*img_height/2);
                ColorPalette _palette = b.Palette;
                Color[] _entries = _palette.Entries;
                if (debug_level == 1)
                    Console.WriteLine("  Grey Pallete Entries Length {0} ", _entries.Length);
                for (int i = 0; i < 256; i++)
                {
                    //Byte d = finalframe[i];
                    Color c = Color.FromArgb(i % 255, i % 255, i % 255);
                    _entries[i] = c;
                }
                b.Palette = _palette;
                System.Drawing.Imaging.BitmapData bData = b.LockBits(rect, System.Drawing.Imaging.ImageLockMode.WriteOnly, b.PixelFormat);
                Marshal.Copy(data, 0, bData.Scan0, data.Length);
                b.UnlockBits(bData);
                if (Form1.active_screen == screen_id)
                {
                    g.DrawImage(b, Form1.border_thickness / 2, Form1.border_thickness / 2, ImgSize * img_width / 2 - Form1.border_thickness, ImgSize * img_height /2- Form1.border_thickness);
                    //g.DrawImage(b, Form1.border_thickness / 2, Form1.border_thickness / 2, img_width/2 - Form1.border_thickness, img_height/2 - Form1.border_thickness);
                }
                else
                {
                    g.DrawImage(b, 0, 0, ImgSize * img_width / 2, ImgSize * img_height /2);
					//g.DrawImage(b, 0, 0,  img_width/2 ,  img_height/2);
				}

                // Stop timing.

                stopwatch.Stop();
                // Write result.
                long milliSec = stopwatch.ElapsedMilliseconds;
                if (debug_level == 1)
                {
                    Console.WriteLine(" Display Time elapsed: {0} msecond", milliSec);
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(" Exception  {0} ", e.ToString());
            }
        }

        void DrawColorBitmapOptimized(PictureBox form, Byte[] data,byte screen_id)
        {
            //Console.WriteLine(" COLORED screenid {0} ", screen_id);
            long milliSec = 0;
            //Console.WriteLine(" Display Step1Time elapsed: {0} msecond", milliSec);

            Stopwatch stopwatch = new Stopwatch();
            stopwatch.Start();
           
            try
            {
                //for (int i = 0; i < img_height * img_width; i++)
                //    Console.WriteLine("PIX {0} ", finalframe[i]);
                Graphics g;
                // Sets g to a graphics object representing the drawing surface of the  
                // control or form g is a member of.  
                g = form.CreateGraphics();
                Bitmap b = new Bitmap(ImgScale*img_width/2, ImgScale*img_height/2, PixelFormat.Format24bppRgb);
                System.Drawing.Rectangle rect = new System.Drawing.Rectangle(0, 0, ImgScale*img_width/2, ImgScale*img_height/2);
                ColorPalette _palette = b.Palette;
                Color[] _entries = _palette.Entries;
                if (debug_level == 1)
                   Console.WriteLine("Color Pallete Entries size {0} ", _entries.Length);
                System.Drawing.Imaging.BitmapData bData = b.LockBits(rect, System.Drawing.Imaging.ImageLockMode.WriteOnly, b.PixelFormat);
                Marshal.Copy(data, 0, bData.Scan0, data.Length);
                b.UnlockBits(bData);
               
                if (Form1.active_screen == screen_id)
                   g.DrawImage(b, Form1.border_thickness/2, Form1.border_thickness/2, ImgSize*img_width/2-Form1.border_thickness,ImgSize*img_height/2-Form1.border_thickness);
                else
                    g.DrawImage(b, 0,0, ImgSize * img_width / 2, ImgSize * img_height / 2);
                stopwatch.Stop();
                // Write result.
                milliSec = stopwatch.ElapsedMilliseconds;
                if (debug_level == 1)
                {
                    Console.WriteLine(" Display Time elapsed: {0} msecond", milliSec);
                }
        
            }
            catch (Exception e)
            {
                Console.WriteLine(" <<<<<<<< Exception  {0} ", e.ToString());
            }
 
        }




        void DrawGreyBitmap(PictureBox form, Byte[] data)
        {
            byte[] finalframe;
            finalframe = new byte[2*img_height * 2*img_width];
            

            long milliSec = 0;
                     
            // Begin timing.
            Stopwatch stopwatch = new Stopwatch();
            stopwatch.Start();
            Graphics g;
            // Sets g to a graphics object representing the drawing surface of the  
            // control or form g is a member of.  
            g = form.CreateGraphics();
            Bitmap b = new Bitmap(img_width, img_height);
            stopwatch.Stop();

            // Write result.
            milliSec = stopwatch.ElapsedMilliseconds;
            if (debug_level == 1)
            {
                Console.WriteLine(" Display Step1Time elapsed: {0} msecond", milliSec);
            }

             stopwatch = new Stopwatch();
            stopwatch.Start();

            for (int y = 0; y < img_height; y++)
            {

                for (int x = 0; x < img_width; x++)
                {
                    Byte d = finalframe[y * img_width + x];
                    Color c = Color.FromArgb(d % 255, d % 255, d % 255);
                    b.SetPixel(x, y, c);
                }
            }
            stopwatch.Stop();

            // Write result.
            milliSec = stopwatch.ElapsedMilliseconds;
            if (debug_level == 1)
            {
                Console.WriteLine(" Display Step2 Time elapsed: {0} msecond", milliSec);
            }
            //form.BackgroundImage = flag;




             stopwatch = new Stopwatch();
            stopwatch.Start();

            System.Drawing.RectangleF rect = new System.Drawing.RectangleF(0, 0, img_width, img_height);
            g.DrawImage(b, rect);
            // Stop timing.

            stopwatch.Stop();
            // Write result.
             milliSec = stopwatch.ElapsedMilliseconds;
            if (debug_level == 1)
            {
                Console.WriteLine(" Display Step3 Time elapsed: {0} msecond", milliSec);
            }
         }

       
        
        void DrawImage(byte[] data,Form form, int W, int H, int step)
        {
           //Graphics g;
            // Sets g to a graphics object representing the drawing surface of the  
            // control or form g is a member of.  
            //g = form.CreateGraphics();
            //System.Drawing.Graphics  graphics = control.CreateGraphics();
            System.IntPtr ptr = BytesToIntptr(data);
            //System.Drawing.Bitmap  b =  new System.Drawing.Bitmap (W, H, step, System.Drawing.Imaging.PixelFormat.Format24bppRgb, ptr);
            System.Drawing.Bitmap b = new System.Drawing.Bitmap(W, H, step, System.Drawing.Imaging.PixelFormat.Format8bppIndexed, ptr);
            form.BackgroundImage = b;
            // System.Drawing.RectangleF rect  = new System.Drawing.RectangleF (0,0,W,H);
            //g.DrawImage(b, rect);
        }

        
        

        public void DisplayVideoOnScreen(PictureBox form,byte [] data, String server, int port, byte screen_id, byte v_id)
        {
            int retry_connect = 0;
            //TcpClient client = null;
            while (true)
            {
                if (retry_connect > MAX_CONNECT_RETRY)
                {
                    return;
                }
                retry_connect++;
                try
                {
                    // Create a TcpClient.
                    // Note, for this client to work you need to have a TcpServer
                    // connected to the same address as specified by the server, port
                    // combination.
                   
                   
                  client = new TcpClient(server, port);
                  Console.WriteLine("DisplayVideoOnScreen1 : retry {0}", retry_connect);
                    

                    


                    // Translate the passed message into ASCII and store it as a Byte array.


                    // Get a client stream for reading and writing.
                    //  Stream stream = client.GetStream();

                    NetworkStream stream = client.GetStream();
                     Console.WriteLine("DisplayVideoOnScreen1: Socket Connection Sucessful ipaddr {0} port  {1}", server, port);
                    // Read the message to the connected TcpServer.
                    int bytesRecv = 0;
                    byte[] buf = new byte[4];
                    int index = 0;
                    while (true)
                    {

                        while (!Form1.active_vehicles[v_id] || !Form1.active_screens[2 * v_id + screen_id])
                        {
                            // sleep nilisconds
                            //Console.WriteLine("waiting vechile {0}", v_id);
                            System.Threading.Thread.Sleep(WAITING_DELAY);  // msec
                            continue;
                        }
                        while (buf[0] != 0xaf && buf[1] != 0xbf && buf[2] != 0xcf && buf[3] != 0xdf)
                        {
                            bytesRecv = stream.Read(buf, 0, 4);
                            index++;
                            // Console.WriteLine("try get start index {0} {1:X} {2:X} {3:X} {4:X}",index, buf[0], buf[1], buf[2], buf[3]);
                        }
                        //Console.WriteLine("vechile {0:X}  screen id {1:X} ", v_id,screen_id);
                        //Console.WriteLine("start {0:X} {1:X} {2:X} {3:X}", buf[0], buf[1],buf [2],buf [3]);
                        buf[0] = buf[1] = buf[2] = buf[3] = 0;
                        bytesRecv = 0;
                        index = 0;
                        //cnt = 0;
						//Console.WriteLine ("start **************");
						while (index != data.Length)
                        {
                            bytesRecv = stream.Read(data, index, data.Length - index);
                            index += bytesRecv;
                            //cnt = (cnt + 1) % 240;
                            //Console.WriteLine("bytes received {0} index {1}", bytesRecv, index);
                            
							//Console.WriteLine("{0},{2},{3:X}", data.Length, cnt, bytesRecv);
						}
						//Console.WriteLine("end **************");
						//Console.WriteLine("Len: {0}  {1}", byteRecv, data.Length);
						index = 0;
                        while (buf[0] != 0xfa && buf[1] != 0xfb && buf[2] != 0xfc && buf[3] != 0xfd)
                        {
                            bytesRecv = stream.Read(buf, 0, 4);
                            //Console.WriteLine(" try get end index {0} {1:X} {2:X} {3:X} {4:X}", index,buf[0], buf[1], buf[2], buf[3]);
                        }

                        //Console.WriteLine(" end {0:X} {1:X} {2:X} {3:X}", buf[0], buf[1], buf[2], buf[3]);
                        buf[0] = buf[1] = buf[2] = buf[3] = 0;
                        if (Form1.elmsize [2*v_id +screen_id] == 1)
                            DrawGreyBitmapOptimized(form, data, screen_id);
                        else
                            DrawColorBitmapOptimized(form, data, screen_id);
						System.Threading.Thread.Sleep(WAITING_DELAY);
					}

                    // Receive the TcpServer.response.
                    //Console.WriteLine("end **************");

					//stream.Close();
					//client.Close();
				}
                catch (ArgumentNullException e)
                {
					retry_connect = 0;
					Console.WriteLine("DisplayVideoOnScreen: ArgumentNullException: Read Fail {0}", e.GetType ());
                }
                catch (SocketException e)
                {
					retry_connect = 0;
					Console.WriteLine("DisplayVideoOnScreen:SocketException: Read Fail {0}", e.GetType ());
                }
                catch (Exception e)
                {
					retry_connect = 0;
					Console.WriteLine("DisplayVideoOnScreen:GeneralException: Read Fail {0}", e.GetType());
                }
                //Console.WriteLine("\n Press Enter to continue...");
                //Console.Read();
            }
        }



        public void DisplayVideoOnScreen(PictureBox form,byte [] data, IPAddress remote, int port,byte screen_id,int v_id)
        {
            int retry_connect = 0;
            bool disconnect_flag = false;
            //Socket s_camera= null;
            while (true)
            {
                disconnect_flag = false;
                if (retry_connect > MAX_CONNECT_RETRY)
                {
                    Console.WriteLine("DisplayVideoOnScreen2: Thread Terminated");
                    break;
                }
                retry_connect++;

                // Connect to a remote device.  
                
                {
                    // Establish the remote endpoint for the socket.  
                    // This example uses port 11000 on the local computer.  
                    //IPHostEntry ipHostInfo = Dns.GetHostEntry(Dns.GetHostName());
                    //IPAddress ipAddress = ipHostInfo.AddressList[0];
                    IPEndPoint remoteEP = new IPEndPoint(remote, port);

                    // Create a TCP/IP  socket.  
                    s_camera = new Socket(remote.AddressFamily, SocketType.Stream, ProtocolType.Tcp);
                    s_camera.ReceiveTimeout = 5000; // msec
                    Console.WriteLine("DisplayVideoOnScreen2: retry {0}", retry_connect);
                    // Connect the socket to the remote endpoint. Catch any errors.  
                    try
                    {
                        s_camera.Connect(remoteEP);

                        Console.WriteLine("DisplayVideoOnScreen:Socket Connection  Successful {0}", s_camera.RemoteEndPoint.ToString());
                        //retry_connect = 0;

                        // recv the data through the socket.  
                        int bytesRecv = 0;
                        int offset = 0;
                        byte[] buf = new byte[4];
                        int index = 0;

                        while (true)
                        {
                            while (!Form1.active_vehicles[v_id] || !Form1.active_screens[2 * v_id + screen_id])
                            {
                                // sleep nilisconds
                                //Console.WriteLine("waiting vechile {0}", v_id);
                                System.Threading.Thread.Sleep(WAITING_DELAY);  // msec
                                continue;
                            }
                            //Console.WriteLine("vechile {0:X}  screen id {1:X} ", v_id, screen_id);
                            Stopwatch stopwatch = Stopwatch.StartNew();
                            stopwatch.Start();
							//System.Threading.Thread.Sleep(10);
							//Console.WriteLine("start **************");
							while (buf[0] != 0xaf && buf[1] != 0xbf && buf[2] != 0xcf && buf[3] != 0xdf)
                            {
								
                                //Console.WriteLine("DisplayVideoOnScreen 2: before receive");
								bytesRecv = s_camera.Receive(buf, 0, 4, SocketFlags.None);
								//Console.WriteLine("DisplayVideoOnScreen2: after receive {0}", bytesRecv);
								index++;
								// Console.WriteLine("try get start index {0} {1:X} {2:X} {3:X} {4:X}", index, buf[0], buf[1], buf[2], buf[3]);
								if (bytesRecv == 0)
								{
									if (s_camera.Poll(1000, SelectMode.SelectRead))
									{
										Console.WriteLine("DisplayVideoOnScreen 2: PIPE Broken {0}", bytesRecv);
										disconnect_flag = true;
										break;
									}
									else
									{
										Console.WriteLine("DisplayVideoOnScreen 2: PIPE OK {0}", bytesRecv);
									}
                                    
								}
							}
                            if (disconnect_flag)
                            {
                                disconnect_flag = false;
                                break;
                            }
                            //Console.WriteLine("start {0:X} {1:X} {2:X} {3:X}", buf[0], buf[1], buf[2], buf[3]);
                            buf[0] = buf[1] = buf[2] = buf[3] = 0;
                            offset = bytesRecv = 0;
                           // cnt = 0;
							
							while (offset != data.Length)
                            {

                                //Console.WriteLine("DisplayVideoOnScreen 2: before receive");
                                bytesRecv = s_camera.Receive(data, offset, data.Length - offset, SocketFlags.None);
                                //Console.WriteLine("DisplayVideoOnScreen2: after receive {0}", bytesRecv);
                                offset += bytesRecv;
								//cnt = (cnt + 1) % 240;
								//Console.WriteLine("{0},{2},{3:X}",data.Length, cnt, bytesRecv);
								if (bytesRecv == 0)
								{
                                    
									if (s_camera.Poll(1000, SelectMode.SelectRead))
									{
										Console.WriteLine("DisplayVideoOnScreen 2: PIPE Broken {0}", bytesRecv);
										disconnect_flag = true;
										break;
									}
									else
									{
										Console.WriteLine("DisplayVideoOnScreen 2: PIPE OK {0}", bytesRecv);
									}
                                   
								};
								
							}
                            if (disconnect_flag)
                            {
                                disconnect_flag = false;
                                break;
                            }
							if (debug_level == 1)
                            {
                                Console.WriteLine("bytes received  {0}  buffer data length{1} ", offset, data.Length);
                            }
                            //for (int i = 0; i < data.Length; i++)
                            //Console.WriteLine("DisplayVideoOnScreen index {0} PIX {1:X} ", i,data[i]);

                            // System.Threading.Thread.Sleep(500);
                            while (buf[0] != 0xfa && buf[1] != 0xfb && buf[2] != 0xfc && buf[3] != 0xfd)
                            {
                                
								bytesRecv = s_camera.Receive(buf, 0, 4, SocketFlags.None);
								//Console.WriteLine("DisplayVideoOnScreen2: after receive {0}", bytesRecv);

								index++;
								//Console.WriteLine("DisplayVideoOnScrean2:try get end index {0} {1:X} {2:X} {3:X} {4:X}", index, buf[0], buf[1], buf[2], buf[3]);
								if (bytesRecv == 0)
								{
                                    /*
                                    if (s_camera.Poll(1000, SelectMode.SelectRead))
                                    {
                                        Console.WriteLine("DisplayVideoOnScreen 2: PIPE Broken {0}",bytesRecv);
                                        disconnect_flag = true;
                                        break;
                                    }
                                    else
                                    {
										Console.WriteLine("DisplayVideoOnScreen 2: PIPE OK {0}",bytesRecv);
									}
                                    */
								}
                                //Console.WriteLine("end **************");
							}
							
							if (disconnect_flag)
							{
								disconnect_flag = false;
								break;
							}
							//Console.WriteLine(" end {0:X} {1:X} {2:X} {3:X}", buf[0], buf[1], buf[2], buf[3]);
							buf[0] = buf[1] = buf[2] = buf[3] = 0;
                            stopwatch.Stop();
                            // Write result.
                            long milliSec = stopwatch.ElapsedMilliseconds;
                            if (debug_level == 1)
                            {
                                Console.WriteLine(" Transmission Time elapsed: {0} msecond", milliSec);
                            }



                            //DrawImage(data, form, 640, 480, 640);

                            if (Form1.elmsize [2*v_id + screen_id] == 1)
                                DrawGreyBitmapOptimized(form, data, screen_id);
                            else
                                DrawColorBitmapOptimized(form, data, screen_id);
							System.Threading.Thread.Sleep(WAITING_DELAY);
							// Console.WriteLine( " stop watch {0} frame {1}",stopwatch.ElapsedMilliseconds,j++);
							// Console.WriteLine("byte received = {0}", bytesRecv);
							// Console.WriteLine("data length = {0}", data.Length);
							// Console.WriteLine("length = {0}", len);
							//Console.WriteLine("LEN = {0}  {0)", leng,len);
						}

                        // Receive the response from the remote device.  
                        //int bytesRec = s.Receive(data);

                    }
                    catch (ArgumentNullException ane)
                    {
                        retry_connect = 0;
                        Console.WriteLine("DisplayVideoOnScreen2:ArgumentNullException reason {0}", ane.GetType());
                        closeSocket(s_camera);
                    }
                    catch (SocketException se)
                    {
                        retry_connect = 0;
                        Console.WriteLine("DisplayVideoOnScreen2: SocketException reason {0}",se.GetType());
                        closeSocket(s_camera);
                    }
                    catch (Exception e)
                    {
                        retry_connect = 0;
                        Console.WriteLine("DisplayVideoOnScreen2 :Unexpected exception {0}", e.GetType ());
                        closeSocket(s_camera);
                    }

                }
                Console.WriteLine("***************   Reconnection will be excuted ******************");    
            }
        }



        

    }


    class ThreadCreationProgram
    {
        Form form = null;
        PictureBox pictbox = null;
        IPAddress remote;
        int port;
        byte screen_id = 0;
        byte v_id = 0;
        int debug_level = 1;
        SurvillenceCamera c = null;
        //int compress_type = SurvillenceCamera.COMPRESS_2x2PIX;
        public  ThreadCreationProgram(Form f)
        {
            form = f;
        }
        
        public ThreadCreationProgram(PictureBox pb, IPAddress _remote, int _port,byte _screen_id,byte _v_id)
        {
            pictbox = pb;
            remote = _remote;
            port = _port;
            screen_id = _screen_id;
            v_id = _v_id;

        }
        public ThreadCreationProgram(Form f, IPAddress _remote, int _port)
        {
            form = f;
            remote = _remote;
            port = _port;
        }
        public ThreadCreationProgram()
		{

		}

        public void shutDown()
        {
            c.closeSocket();
        }
        public void thread()
		{
            if (debug_level == 1)
                 Console.WriteLine("Child thread starts");
            
            
           // IPAddress remote = IPAddress.Parse("192.168.1.130");
            //IPAddress remote = IPAddress.Parse("25.27.164.23");
            //int port = 4095;
            c = new SurvillenceCamera(pictbox,remote,port,screen_id,v_id);

            //c.DisplayVideoOnScreen(pictbox, data, "192.168.1.130", port);
            //c.DisplayVideoOnScreen(pictbox, data, remote, port);
        }
    }
   static class Program
	{
        /// <summary>
        /// The main entry point for the application.
        private const int CTR_OFFSET = 250;  // change the size of Motion window
        /// </summary>
        [STAThread]
		static void Main()
		{
            
  
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Form1 form = new Form1();
            form.Size = new Size(4 * SurvillenceCamera.img_width+20, 800+Form1.MENU_HIGHT);
            form.BackColor = Color.OrangeRed;
            Application.Run(form);
            Console.WriteLine("TEST4");
            // Data buffer for incoming data.  
            
       }
    }
}
