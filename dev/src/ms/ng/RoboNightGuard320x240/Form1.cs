using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;
using System.Diagnostics;
using System.Threading;
using System.Timers;
//using SharpDX;
using SharpDX.DirectInput;
using System.Linq.Expressions;
using System.Runtime.InteropServices;
using System.Net.Http.Headers;

namespace RoboNightGuard
{
	public class Joystick_c
	{
		static Joystick joystick;
		static System.Timers.Timer timer;
		static JoystickState previousState;
		static int c = 0;
		NetworkStream[] MotionControlStream = { null, null, null, null };
		int active_vehicle = 0;
		Byte[] data = new byte[4];

		const byte MV_FRWD = 0x21;
		const byte MV_BKWD = 0x31;
		const byte MV_LEFT = 0x41;
		const byte MV_RGHT = 0x51;
		const byte F_CAMERA_UP = 0x52;
		const byte F_CAMERA_DN = 0x53;
		const byte F_CAMERA_LFT = 0x54;
		const byte F_CAMERA_RGT = 0x55;
		const byte B_CAMERA_UP = 0x56;
		const byte B_CAMERA_DN = 0x57;
		const byte B_CAMERA_LFT = 0x58;
		const byte B_CAMERA_RGT = 0x59;
		const byte STOP = 0x61;
		const byte STOP_MOTION = 0x62;  // stop wheels
		const byte ACK = 0x71;
		const byte UP_SPEED = 0x81;
		const byte DN_SPEED = 0x91;
		const byte SPEED = 0x92;
		const byte END_RECORD = 0x93;
		const byte START_RECORD = 0x94;
		const byte ROOM_NO = 0x95;
		const byte REPLAY_MOTION = 0x96;
		const byte REVERSE_MOTION = 0x97;
		const byte ZOOM_CMD = 0x98;

		private void send_cmd(byte cmd)
		{
			data[0] = cmd;

			try
			{
				if (MotionControlStream[active_vehicle] != null)
					MotionControlStream[active_vehicle].Write(data, 0, 4);
				//strm.Write(data, 0, 4);
			}
			catch (ArgumentNullException ane)
			{
				Console.WriteLine("ArgumentNullException : {0}", ane.ToString());
			}
			catch (SocketException se)
			{
				Console.WriteLine("SocketException : {0}", se.ToString());
			}
			catch (Exception ex)
			{
				Console.WriteLine("Unexpected exception : {0}", ex.ToString());
			}

		}


		private void init_procedure()
		{
			var directInput = new DirectInput();
			if (directInput == null)
				return;
			// Find a connected joystick device
			var joystickGuid = Guid.Empty;
			//DeviceType.Keyboard;
			foreach (var deviceInstance in directInput.GetDevices(DeviceType.Gamepad, DeviceEnumerationFlags.AllDevices))
			{
				Console.WriteLine("device found ");
				joystickGuid = deviceInstance.InstanceGuid;
				break;
			}
			// Create a new joystick device
			joystick = new Joystick(directInput, joystickGuid);
			// Acquire the device
			joystick.Properties.BufferSize = 128;
			joystick.Acquire();
			// set the previous state of the joystick
			previousState = joystick.GetCurrentState();
			Console.WriteLine("previous state" + previousState);
			// Create a Timer object that knows to call our TimerCallback
			// method once every 1000 milliseconds.
			timer = new System.Timers.Timer(1000);
			timer.Elapsed += new ElapsedEventHandler(TimerCallback);
			timer.Enabled = true;

			// Wait for the user to hit <Enter>
			Console.WriteLine("Press the Enter key to exit the program.");
			Console.ReadLine();
		}
		public Joystick_c()
		{
			init_procedure();
		}
		public Joystick_c(NetworkStream[] _MotionControlStream)
		{
			init_procedure();
			MotionControlStream = _MotionControlStream;
		}

		private void TimerCallback(Object o, ElapsedEventArgs e)
		{
			//Console.WriteLine("xxxxxx enter callback " + c++);
			JoystickState state = joystick.GetCurrentState();
			for (int i = 0; i < state.Buttons.Length; i++)
			{

				//if (i <= 7 && state.Buttons[i])
				//Console.WriteLine("Button " + i + " state = " + state.Buttons[i] + " count = " + c);
				if (previousState.Buttons[i] != state.Buttons[i])
				{
					if (state.Buttons[i])
					{
						Console.WriteLine("Button " + i + " pressed.");
					}
					else
					{
						Console.WriteLine("Button " + i + " released.");
					}
				}

				if (previousState.X != state.X)
				{
					//Console.WriteLine("Cnt " + c + " X = " + Math.Round((double)((state.X/32768.0f)-1)));
					Console.WriteLine("Cnt " + c + " X = " + Math.Round(((double)state.X * (double)64 / (2.0f * 32768.0f))));
				}
				if (previousState.Y != state.Y)
				{
					//Console.WriteLine("Cnt " + c + " Y = " + Math.Round((double)((state.Y/32768.0f)-1)));
					Console.WriteLine("Cnt " + c + "  Y = " + Math.Round(((double)state.Y * (double)64 / (2.0f * 32768.0f))));
				}

				if (previousState.Z != state.Z)
				{
					Console.WriteLine("Cnt " + c + " Z = " + state.Z);
				}


				if (previousState.AngularVelocityX != state.AngularAccelerationX)
				{
					Console.WriteLine("Cnt " + c + " Angular X = " + state.AngularAccelerationX);
				}


				if (previousState.AccelerationX != state.AccelerationX)
				{
					Console.WriteLine("Cnt " + c + " Accelrate X = " + state.AccelerationX);
				}
				if (previousState.AccelerationY != state.AccelerationY)
				{
					Console.WriteLine("Cnt " + c + " Accelrate Y = " + state.AccelerationY);
				}

				if (previousState.AngularVelocityX != state.AngularVelocityX)
				{
					Console.WriteLine("Cnt " + c + " Angular Velocity X = " + state.AngularVelocityX);
				}

				if (previousState.AngularVelocityY != state.AngularVelocityY)
				{
					Console.WriteLine("Cnt " + c + " Angular Velocity Y = " + state.AngularVelocityY);
				}

				if (previousState.ForceX != state.ForceX)
				{
					Console.WriteLine("Cnt " + c + " Force X = " + state.ForceX);
				}

				if (previousState.ForceY != state.ForceY)
				{
					Console.WriteLine("Cnt " + c + " Force Y = " + state.ForceY);
				}

				if (previousState.RotationX != state.RotationX)
				{
					// Console.WriteLine("Cnt " + c + " Rotate X = " + Math.Round((double)((state.RotationX / 32768.0f) - 1)));

					Console.WriteLine("Cnt " + c + " Rotate X = " + Math.Round(((double)state.RotationX * (double)64 / (2.0f * 32768.0f))));
				}

				if (previousState.RotationY != state.RotationY)
				{
					//Console.WriteLine("Cnt " + c + " Rotate Y = " + Math.Round((double)((state.RotationY / 32768.0f) - 1)));
					Console.WriteLine("rotation " + c + " Rotate Y = " + Math.Round(((double)state.RotationY * (double)64 / (2.0f * 32768.0f))));
				}

				if (previousState.TorqueX != state.TorqueX)
				{
					Console.WriteLine("Cnt " + c + " Torque X = " + state.TorqueX);
				}
				if (previousState.TorqueY != state.TorqueY)
				{
					Console.WriteLine("Cnt " + c + " Torque Y = " + state.TorqueY);
				}
				if (previousState.VelocityX != state.VelocityX)
				{
					Console.WriteLine("Cnt " + c + " Velocity X = " + state.VelocityX);
				}
				if (previousState.VelocityY != state.VelocityY)
				{
					Console.WriteLine("Cnt " + c + " Velocity Y = " + state.VelocityY);
				}

			}

			previousState = state;
		}


	}

	public partial class Form1 : Form
	{

		const int CLIENTS_NUM = 6;
		String[] server = { "192.168.12.116", "192.168.12.116" };
		//String[] server = { "192.168.1.130", "192.168.1.`130" };
		int[] port = {
						 4095,    // 0 front video display v1
			             4098,    // 1 front video control v1 
						 14095,   // 2 back video display  v1
			             14098,   // 3 back video control  v1
			             4099     // 4 mot:/portion control v1
		               };
		public static byte active_vehicle = 0;
		public static int active_screen = 0;
		public static int x = 0;
		public static int border_thickness = 20;
		TcpClient[] VideoControlClients = {
											 null, null,    //  video front and back v1
			                                 null, null     // video front and back  v2
			                               };
		NetworkStream[] VideoControlStream = {
											   null, null,   // front and back v1 
			                                   null, null    // front and back v2
		                                     };
		TcpClient[] MotionControlClients =   {
											   null,  // motion ctr v1
			                                   null,  // motion ctr v2
		                                    };

		NetworkStream[] MotionControlStream = { null, null };
		ThreadCreationProgram[] f_client = { null, null };
		ThreadStart[] childref_f_client = { null, null };
		Thread[] f_clientThread = { null, null };
		ThreadCreationProgram[] b_client = { null, null };
		ThreadStart[] childref_b_client = { null, null };
		Thread[] b_clientThread = { null, null };
		Thread[] video_control_thread = { null, null };  // front and back camera for max 2 vechiles
		Thread[] motion_control_thread = { null, null };  // max 2 vechiles
		Byte[] data = new byte[4];
		const int ZOOM_MAX = 10;
		const int ZOOM_MIN = 1;
		const byte MIN_SPEED = 50;
		const byte MAX_SPEED = 254;
		const byte SPEED_INC = 10;
		int zoom_level = ZOOM_MIN;
		const byte MAX_VEHICLES = 2;
		private Point[] ArrowPoints;
		const int DIVIDE_X = 10;
		const int DIVIDE_Y = 10;
		public static int MENU_HIGHT = 55;
		private byte speed = MIN_SPEED;
		//public static byte active_vehicle = 0;

		const byte MV_FRWD = 0x21;
		const byte MV_BKWD = 0x31;
		const byte MV_LEFT = 0x41;
		const byte MV_RGHT = 0x51;
		const byte F_CAMERA_UP = 0x52;
		const byte F_CAMERA_DN = 0x53;
		const byte F_CAMERA_LFT = 0x54;
		const byte F_CAMERA_RGT = 0x55;
		const byte B_CAMERA_UP = 0x56;
		const byte B_CAMERA_DN = 0x57;
		const byte B_CAMERA_LFT = 0x58;
		const byte B_CAMERA_RGT = 0x59;
		const byte STOP = 0x61;
		const byte STOP_MOTION = 0x62;  // stop wheels
		const byte ACK = 0x71;
		const byte UP_SPEED = 0x81;
		const byte DN_SPEED = 0x91;
		const byte SPEED = 0x92;
		const byte END_RECORD = 0x93;
		const byte START_RECORD = 0x94;
		const byte ROOM_NO = 0x95;
		const byte REPLAY_MOTION = 0x96;
		const byte REVERSE_MOTION = 0x97;
		const byte ZOOM_CMD = 0x98;
		const int MAX_CONNECT_RETRY= 10000;
        private const int WAITING_DELAY = 15000;     // msecond


        private const int CTR_OFFSET = 250;  // change the size of Motion window
		PictureBox f_camera = null;
		PictureBox b_camera = null;

		Joystick_c joystick;
		// 1 for grey and 3 for color
		public static int[] elmsize = {
										 1,   // front camera v1
                                         1,   // back camera v1
                                         1,   // front camera v2
                                         1,   // back camera v2
                                         1,   // front camera v3
                                         1,   // back camera v3
                                      };



		public static bool[] active_vehicles = {
													true,      // v1 by default
												    false,     // v2 
													false,     // v3
													false      // v4
		                                         };

		public static bool[] active_screens = {
												   	true,      // v1  front by default
												    true,     // v1  back  by default
													false,     // v2  front 
													false,     // v2   back
				                                    false,     // v3  front 
													false,     // v3   back
													false,     // v4  front 
											    	false,     // v4   back
		                                         };
		//public static bool color_frame_change = false;
		//public static byte current_elmsize = 1;

		public Form1()
		{
			InitializeComponent();

		}



		private void video_control(int vec)
		{
			int retry_connect = 0;
			Console.WriteLine("START  video control Thread");
			while (true)
			{
				if (retry_connect > MAX_CONNECT_RETRY)
				{
					break;
				}
				retry_connect++;

				System.Threading.Thread.Sleep(WAITING_DELAY);
				try
				{
					// v1  video control front camera  
					Console.WriteLine("video_control: retry count {0}", retry_connect);
					VideoControlClients[2 * vec] = new TcpClient(server[vec], port[1]);
					VideoControlStream[2 * vec] = VideoControlClients[2 * vec].GetStream();
					Console.WriteLine("video control: front  succesful");
					
					// vec video control back camera 
					VideoControlClients[2 * vec + 1] = new TcpClient(server[vec], port[3]);
					VideoControlStream[2 * vec + 1] = VideoControlClients[2 * vec + 1].GetStream();
					
					if (!VideoControlClients[2 * vec].Connected || !VideoControlClients[2 * vec + 1].Connected)
					{
						if (VideoControlClients[2 * vec] != null)
							// Close the client if it's not connected
							VideoControlClients[2 * vec].Close();
						if (VideoControlClients[2 * vec + 1] != null)
							VideoControlClients[2 * vec + 1].Close();
						Console.WriteLine("video_control: Connection Fail :Video Control ipaddr {0} front port  {1} back port {2}  reason {3}", server[vec], port[1], port[3]);
						continue;
					}
				
				    Console.WriteLine("video_control:  Connection Successful :Video Control ipaddr {0} front port  {1} back port {2}", server[vec], port[1], port[3]);
					Console.WriteLine("video_control: start recevie cmd");
		           //	while( true)
				   /*
)					{
						System.Threading.Thread.Sleep(WAITING_DELAY);
						Console.WriteLine("video_control: receive cmd");
						retry_connect = 0;
						if (!VideoControlClients[2 * vec].Connected || !VideoControlClients[2 * vec + 1].Connected)
						{
							if (VideoControlClients[2 * vec] != null)
								// Close the client if it's not connected
								VideoControlClients[2 * vec].Close();
							if (VideoControlClients[2 * vec + 1] != null)
								VideoControlClients[2 * vec + 1].Close();
							Console.WriteLine("video_control: Connection Fail :Video Control ipaddr {0} front port  {1} back port {2}  reason {3}", server[vec], port[1], port[3]);
							break;
						}
						bool canRead = VideoControlClients[2 * vec].Client.Poll(0, SelectMode.SelectRead);
						bool canWrite = VideoControlClients[2 * vec].Client.Poll(0, SelectMode.SelectWrite);

						if (!canRead || !canWrite)
						{


							// Close the client if it's not connected
							if (VideoControlClients[2 * vec] != null)
								// Close the client if it's not connected
								VideoControlClients[2 * vec].Close();
							if (VideoControlClients[2 * vec + 1] != null)
								VideoControlClients[2 * vec + 1].Close();
							Console.WriteLine("video_control:Connection Fail: Video Control ipaddr {0} front port  {1} back port {2}  reason {3}", server[vec], port[1], port[3]);
							break;
						}
						canRead = VideoControlClients[2 * vec + 1].Client.Poll(0, SelectMode.SelectRead);
						canWrite = VideoControlClients[2 * vec + 1].Client.Poll(0, SelectMode.SelectWrite);

						if (!canRead || !canWrite)
						{
							// Close the client if it's not connected
							if (VideoControlClients[2 * vec] != null)
								// Close the client if it's not connected
								VideoControlClients[2 * vec].Close();
							if (VideoControlClients[2 * vec + 1] != null)
								VideoControlClients[2 * vec + 1].Close();
							Console.WriteLine("video_control: connection fail Video Control ipaddr {0} front port  {1} back port {2}  reason {3}", server[vec], port[1], port[3]);
							break;
						}
					}
					
					*/
				}


				catch (Exception se)
				{
					Console.WriteLine("video_control:GeneralException : Video Control fail ipaddr {0} front port {1} back port {2} reason {3}", server[vec], port[1], port[3], se.GetType());
					if (VideoControlClients[2 * vec] != null)
						// Close the client if it's not connected
						VideoControlClients[2 * vec].Close();
					if (VideoControlClients[2 * vec + 1] != null)
						VideoControlClients[2 * vec + 1].Close();
				}
				  
			}
		
	}



	


        
        private void motion_control(int vec)
        {
                int retry_connect = 0;
			Console.WriteLine("START montion control Thread");
                while (true)
                {
				  if (retry_connect > MAX_CONNECT_RETRY)
					{
						break;
					}
                retry_connect++;
                System.Threading.Thread.Sleep(WAITING_DELAY);
                try
                    {

                        Console.WriteLine("motion_control: retry count {0}", retry_connect);
                    // v1  video control front camera   
                        MotionControlClients[2 * vec] = new TcpClient(server[vec], port[4]);
                        MotionControlStream[vec] = VideoControlClients[vec].GetStream();
                        // vec video control back camera 
                        if (!MotionControlClients[vec].Connected)
                        {
                        if (MotionControlClients[vec] != null)
                            // Close the client if it's not connected
                            MotionControlClients[vec].Close();
                        Console.WriteLine("motion_control:Connection Fail : motion Control ipaddr {0} port  {1}", server[vec], port[4]);
                            continue;
                        }
                        
                    Console.WriteLine("motion_control: Connection Successful : motion Control ipaddr {0} port  {1}", server[vec], port[4]);
                    while (true)
                    {
                        retry_connect = 0;
						System.Threading.Thread.Sleep(WAITING_DELAY);
						if (!MotionControlClients[vec].Connected)
                        {
                            if (MotionControlClients[vec] != null)
                                // Close the client if it's not connected
                                MotionControlClients[vec].Close();
                            Console.WriteLine("motion_control:Connection Fail : motion Control ipaddr {0} port  {1}", server[vec], port[4]);
                            break;
                        }

                        bool canRead = MotionControlClients[vec].Client.Poll(0, SelectMode.SelectRead);
                        bool canWrite = MotionControlClients[vec].Client.Poll(0, SelectMode.SelectWrite);

                        if (!canRead || !canWrite)
                        {
							if (MotionControlClients[vec] != null)
                            // Close the client if it's not connected
								 MotionControlClients[vec].Close();
                            Console.WriteLine("motion_control:GeneralException : motion Control ipaddr {0} port  {1}", server[vec], port[4]);
                            break;
                        }

                    }

                }
                    catch (Exception se)
                    {
                        Console.WriteLine("motion_control:GeneralException : motion Control ipaddr {0} port  {1} reason {2}", server[vec], port[4], se.GetType());
                    if (MotionControlClients[vec] != null)
                        // Close the client if it's not connected
                        MotionControlClients[vec].Close();

                    }
                }
            }
        

        



        private void create_threads()
		{

			Console.WriteLine("In Main: Creating the Child thread");
			for (int vec = 0; vec < 1; vec++)
			{
				//vec = 1;
				IPAddress remote_server = IPAddress.Parse(server[vec]);
				//IPAddress remote_server = IPAddress.Parse("25.69.221.235");
				//remote_server = IPAddress.Parse("25.8.177.104");
				//remote_server = IPAddress.Parse("192.168.1.130");
				int f_client_port = port[0];
				Console.WriteLine("  before front camera ");
				f_client[vec] = new ThreadCreationProgram(f_camera, remote_server, f_client_port, 0, (byte)vec);
				childref_f_client[vec] = new ThreadStart(f_client[vec].thread); //thread delegate
				f_clientThread[vec] = new Thread(childref_f_client[vec]);
				f_clientThread[vec].Start();

				int b_client_port1 = port[2];
				b_client[vec] = new ThreadCreationProgram(b_camera, remote_server, b_client_port1, 1, (byte)vec);
				childref_b_client[vec] = new ThreadStart(b_client[vec].thread);
				b_clientThread[vec]= new Thread(childref_b_client[vec]);
				b_clientThread[vec].Start();
				/*  DEBUG
				video_control_thread [vec] =  new Thread(delegate ()
                {
                    video_control (vec);
                });
				video_control_thread[vec].Start();

                motion_control_thread[vec] = new Thread(delegate ()
                {
                   motion_control(vec);
                });
				motion_control_thread[vec].Start();
				*/

                    /*
                try
				{
					// v1  video control for front, back  cameras, motion control  
					VideoControlClients[2 * vec] = new TcpClient(server[vec], port[1]);
					VideoControlStream[2 * vec] = VideoControlClients[2 * vec].GetStream();
				}
				catch (SocketException se)
				{
					Console.WriteLine("create_threads:SocketException : front Video Control ipaddr {0} port  {1}", server[vec], port[1]);

				}
				// back camera

				try
				{

					VideoControlClients[2 * vec + 1] = new TcpClient(server[vec], port[3]);
					VideoControlStream[2 * vec + 1] = VideoControlClients[2 * vec + 1].GetStream();
				}
				catch (Exception se)
				{
					Console.WriteLine("create_threads:GeneralException : Back Video Control ipaddr {0} port  {1} reason {2}", server[vec], port[3],se.GetType());
				}

				try
				{
					MotionControlClients[vec] = new TcpClient(server[vec], port[4]);
					MotionControlStream[vec] = MotionControlClients[vec].GetStream();


				}

				catch (Exception se)
				{
					Console.WriteLine("create_threads:SocketException : Motion Control ipaddr {0} port  {1}  reason {2}", server[vec], port[4],se.GetType());
				}
					*/

            }
		}


        private void Form1_Load(object sender, EventArgs e)
		{
			Console.WriteLine("Form1_Load");

			
			try
			{
                //joystick = new Joystick_c();
                active_screens = new bool[2 * MAX_VEHICLES];
				active_screens [0] = true;
				active_screens[1] = true;
				active_vehicles = new bool[MAX_SPEED];
				active_vehicles[0] = true;
				SurvillenceCamera sc = new SurvillenceCamera();
			
				

			    f_camera = new PictureBox();
				f_camera.BackColor = Color.FromArgb(100, 0, 0);
				this.Controls.Add(f_camera);
				f_camera.Size = new Size(sc.ImgSize * SurvillenceCamera.img_width / 2, sc.ImgSize * SurvillenceCamera.img_height / 2);
				f_camera.Location = new Point(0, MENU_HIGHT);
				
				f_camera.DoubleClick += new EventHandler(frontscreen_Click);
				f_camera.Paint += new PaintEventHandler(frontscreen_border_Paint);


				
				b_camera = new PictureBox();
				b_camera.BackColor = Color.FromArgb(0, 0, 55);
				this.Controls.Add(b_camera);
				b_camera.Size = new Size(sc.ImgSize * SurvillenceCamera.img_width / 2, sc.ImgSize * SurvillenceCamera.img_height / 2);
				b_camera.Location = new Point(sc.ImgSize * SurvillenceCamera.img_width / 2, MENU_HIGHT);
				
				b_camera.DoubleClick += new EventHandler(backscreen_Click);
				b_camera.Paint += new PaintEventHandler(backscreen_border_Paint);
				Console.WriteLine("  back line width {0} height {1}", b_camera.Width, b_camera.Height);


				

				




				GroupBox camera_ctr = new GroupBox();
				//Panel  camera_ctr = new Panel();
				camera_ctr.Name = "Panel1";
				camera_ctr.BackColor = Color.FromArgb(50, 60, 13);
				this.Controls.Add(camera_ctr);
				camera_ctr.Size = new Size(sc.ImgSize * SurvillenceCamera.img_width / 2 - CTR_OFFSET, sc.ImgSize * SurvillenceCamera.img_height / 2);
				camera_ctr.Location = new Point(sc.ImgSize * SurvillenceCamera.img_width / 2 + CTR_OFFSET, sc.ImgSize * SurvillenceCamera.img_height / 2+MENU_HIGHT);
				camera_ctr.Text = "Camera Control";
				camera_ctr.Font = new Font("Arial", 20, FontStyle.Bold);
				camera_ctr.ForeColor = Color.Red;
				int m_ctr_w = camera_ctr.Width;
				int m_ctr_h = camera_ctr.Height;
				int b_w = 70;
				int b_h = 70;

				Label zoom_label = new Label();
				zoom_label.Size = new Size(2 * b_w, 30);
				zoom_label.Text = "ZOOM";
				camera_ctr.Controls.Add(zoom_label);
				zoom_label.BackColor = Color.FromArgb(50, 60, 13);
				zoom_label.ForeColor = Color.White;
				zoom_label.Location = new Point(m_ctr_w - (int)(2 * b_w), (int)(m_ctr_h / 12));
				zoom_label.Font = new Font("Arial", 12, FontStyle.Bold);
				zoom_label.TextAlign = ContentAlignment.MiddleCenter;

				Button zoom_in = new Button();
				zoom_in.Size = new Size(b_w, b_h);
				zoom_in.BackColor = Color.FromArgb(255, 204, 154);
				camera_ctr.Controls.Add(zoom_in);
				zoom_in.Location = new Point(m_ctr_w - (int)(1.5 * b_w), (int)(m_ctr_h / 6));
				addCallbackZoomIn(zoom_in);

				Button zoom_out = new Button();
				zoom_out.BackColor = Color.FromArgb(255, 204, 154);
				zoom_out.Size = new Size(b_w, b_h);
				camera_ctr.Controls.Add(zoom_out);
				zoom_out.Location = new Point(m_ctr_w - (int)(1.5 * b_w), (m_ctr_h) / 6 + b_h + 10);
				addCallbackZoomOut(zoom_out);



				//camera_ctr.DropDownStyle = ComboBoxStyle.DropDown;


				// Create the ToolBar, ToolBarButton controls, and menus.

				ToolBar toolBar1 = new ToolBar();
				toolBar1.Size = new Size(4 * SurvillenceCamera.img_width + 20, 100);
				toolBar1.Font = new Font("Ariel", 10, FontStyle.Bold);
				toolBar1.ButtonSize = new Size(20, 80);

				ToolBarButton toolBarButton1 = new ToolBarButton();
				ToolBarButton toolBarButton2 = new ToolBarButton();
				ToolBarButton toolBarButton3 = new ToolBarButton();

				// Set the Text properties of the ToolBarButton controls.
				toolBarButton1.Text = "Vehicle";
				toolBarButton2.Text = "Screen";
				//toolBarButton3.Text = "Print";
				toolBar1.Buttons.Add(toolBarButton1);
				toolBar1.Buttons.Add(toolBarButton2);
				//toolBar1.Buttons.Add(toolBarButton3);

				// Vehicle Selection
				MenuItem VEmenuItem1 = new MenuItem("Vehicle1");
				addCallbackSelectVehicle1(VEmenuItem1);

				MenuItem VEmenuItem2 = new MenuItem("Vehicle2");
				addCallbackSelectVehicle2(VEmenuItem2);

				ContextMenu VEcontextMenu = new ContextMenu(new MenuItem[] { VEmenuItem1, VEmenuItem2 });


				toolBarButton1.Style = ToolBarButtonStyle.DropDownButton;
				toolBarButton1.ImageIndex = 0;
				toolBarButton1.DropDownMenu = VEcontextMenu;

				// Screen Selection
				MenuItem SCmenuItem1 = new MenuItem("Front Screen");
				addCallbackFrontScreen(SCmenuItem1);
	
				MenuItem SCmenuItem2 = new MenuItem("Back Screen");
				addCallbackBackScreen(SCmenuItem2);

				ContextMenu SCcontextMenu = new ContextMenu(new MenuItem[] { SCmenuItem1,SCmenuItem2 });
				

				toolBarButton2.Style = ToolBarButtonStyle.DropDownButton;
				toolBarButton2.ImageIndex = 0;
				toolBarButton2.DropDownMenu = SCcontextMenu;

				

				Controls.Add(toolBar1);


				GroupBox motion_ctr = new GroupBox();
				motion_ctr.Name = "Panel2";
				motion_ctr.BackColor = Color.FromArgb(50, 60, 13);
				Controls.Add(motion_ctr);
				motion_ctr.Size = new Size(sc.ImgSize * SurvillenceCamera.img_width / 2 + CTR_OFFSET, sc.ImgSize * SurvillenceCamera.img_height / 2);
				motion_ctr.Location = new Point(0, sc.ImgSize * SurvillenceCamera.img_height / 2+MENU_HIGHT);
				motion_ctr.Text = "Motion Control";
				motion_ctr.Font = new Font("Arial", 20, FontStyle.Bold);
				motion_ctr.ForeColor = Color.Red;
				m_ctr_w = motion_ctr.Width;
				m_ctr_h = motion_ctr.Height;
				b_w = 70;
				b_h = 70;

				Label speed_label = new Label();
				speed_label.Size = new Size(2 * b_w, 30);
				speed_label.Text = "SPEED";
				motion_ctr.Controls.Add(speed_label);
				speed_label.BackColor = Color.FromArgb(50, 60, 13);
				speed_label.ForeColor = Color.White;
				speed_label.Location = new Point(m_ctr_w - (int)(2 * b_w), (int)(m_ctr_h / 12));
				speed_label.Font = new Font("Ariel", 12, FontStyle.Bold);
				speed_label.TextAlign = ContentAlignment.MiddleCenter;

				Label direction_label = new Label();
				direction_label.Size = new Size(4 * b_w, 30);
				direction_label.Text = "DIRECTION";
				motion_ctr.Controls.Add(direction_label);
				direction_label.BackColor = Color.FromArgb(50, 60, 13);
				direction_label.ForeColor = Color.White;
				direction_label.Location = new Point(m_ctr_w - (int)(5.5 * b_w), (int)(m_ctr_h / 12));
				direction_label.Font = new Font("Ariel", 12, FontStyle.Bold);
				direction_label.TextAlign = ContentAlignment.MiddleCenter;



				Console.WriteLine("TEST4 W {0} {1}  \n", m_ctr_w, m_ctr_h);
				Button speed_up = new Button();
				speed_up.Size = new Size(b_w, b_h);
				speed_up.BackColor = Color.FromArgb(255, 204, 154);
				motion_ctr.Controls.Add(speed_up);
				speed_up.Location = new Point(m_ctr_w - (int)(1.5 * b_w), (int)(m_ctr_h / 6));
				this.addCallbackSpeedUp(speed_up);


				Button speed_down = new Button();
				speed_down.BackColor = Color.FromArgb(255, 204, 154);
				speed_down.Size = new Size(b_w, b_h);
				motion_ctr.Controls.Add(speed_down);
				speed_down.Location = new Point(m_ctr_w - (int)(1.5 * b_w), (m_ctr_h) / 6 + b_h + 10);
				addCallbackSpeedDown(speed_down);

				Button forward = new Button();
				forward.Size = new Size(b_w, b_h);
				forward.BackColor = Color.FromArgb(255, 204, 154);
				motion_ctr.Controls.Add(forward);
				forward.Location = new Point(m_ctr_w - (int)(4 * b_w), (int)(m_ctr_h / 6));
				addCallbackForward(forward);

				Button reverse = new Button();
				reverse.BackColor = Color.FromArgb(255, 204, 154);
				reverse.Size = new Size(b_w, b_h);
				motion_ctr.Controls.Add(reverse);
				reverse.Location = new Point(m_ctr_w - (int)(4 * b_w), (m_ctr_h) / 6 + b_h + 10);
				addCallbackReverse(reverse);

				Button left = new Button();
				left.Size = new Size(b_w, b_h);
				left.BackColor = Color.FromArgb(255, 204, 154);
				motion_ctr.Controls.Add(left);
				left.Location = new Point(m_ctr_w - (int)(5 * b_w + 10), (int)(m_ctr_h / 4));
				addCallbackLeft(left);

				Button right = new Button();
				right.BackColor = Color.FromArgb(255, 204, 154);
				right.Size = new Size(b_w, b_h);
				motion_ctr.Controls.Add(right);
				right.Location = new Point(m_ctr_w - (int)(3 * b_w - 10), ((m_ctr_h) / 4));
				addCallbackRight(right);

				Button stop = new Button();
				stop.Size = new Size(b_w, b_h);
				stop.BackColor = Color.FromArgb(255, 204, 154);
				stop.Font = new Font("Ariel", 12, FontStyle.Bold);
				stop.Text = "STOP";
				motion_ctr.Controls.Add(stop);
				stop.Location = new Point(m_ctr_w - (int)(6 * b_w) - 20, (int)(m_ctr_h / 4));
				addCallbackStop(stop);

				int train_b_w = 100;
				int train_b_h = 30;
				Label training_label = new Label();
				training_label.Size = new Size(2 * b_w, 40);
				training_label.Text = "TRAINING";
				motion_ctr.Controls.Add(training_label);
				training_label.BackColor = Color.FromArgb(50, 60, 13);
				training_label.ForeColor = Color.White;
				training_label.Location = new Point(m_ctr_w - (int)(8 * b_w), (int)(m_ctr_h / 12));
				training_label.Font = new Font("Arial", 12, FontStyle.Bold);
				training_label.TextAlign = ContentAlignment.MiddleCenter;

				Button start_training = new Button();
				start_training.Size = new Size(train_b_w, train_b_h);
				start_training.BackColor = Color.FromArgb(255, 204, 154);
				start_training.Text = " Start";
				motion_ctr.Controls.Add(start_training);
				start_training.Location = new Point(m_ctr_w - (int)(7.8 * (float)b_w), (int)(m_ctr_h / 6));
				start_training.Font = new Font("Ariel", 11, FontStyle.Bold);
				addCallbackStartTraining(start_training);

				Button stop_training = new Button();
				stop_training.BackColor = Color.FromArgb(255, 204, 154);
				stop_training.Size = new Size(train_b_w, train_b_h);
				stop_training.Text = " Stop";
				motion_ctr.Controls.Add(stop_training);
				stop_training.Font = new Font("Ariel", 11, FontStyle.Bold);
				stop_training.Location = new Point(m_ctr_w - (int)(7.8 * (float)b_w), (m_ctr_h) / 6 + train_b_h + 5);
				addCallbackStopTraining(stop_training);

				Button replay_training = new Button();
				replay_training.BackColor = Color.FromArgb(255, 204, 154);
				replay_training.Size = new Size(train_b_w, train_b_h);
				replay_training.Text = "Replay";
				motion_ctr.Controls.Add(replay_training);
				replay_training.Font = new Font("Ariel", 11, FontStyle.Bold);
				replay_training.Location = new Point(m_ctr_w - (int)(7.8 * (float)b_w), (m_ctr_h) / 6 + 2 * train_b_h + 10);
				addCallbackReplay(replay_training);

		
				Button goback_training = new Button();
				goback_training.BackColor = Color.FromArgb(255, 204, 154);
				goback_training.Size = new Size(train_b_w, train_b_h);
				goback_training.Text = "Go Back";
				motion_ctr.Controls.Add(goback_training);
				goback_training.Font = new Font("Ariel", 11, FontStyle.Bold);
				goback_training.Location = new Point(m_ctr_w - (int)(7.8 * (float)b_w), (m_ctr_h) / 6 + 3 * train_b_h + 15);
				addCallbackGoback(goback_training);


				Console.WriteLine("In Main: Creating the Child thread");
				
				//joystick = new Joystick_c(MotionControlStream);
            }
			catch (ArgumentNullException ane)
			{
				Console.WriteLine("ArgumentNullException : {0}", ane.ToString());

			}
						
			catch (Exception ex)
			{
				Console.WriteLine("Unexpected exception : {0}", ex.ToString());
     		}



			Console.WriteLine("In Main: Creating the Child thread");
			create_threads();
			/*
			for (int vec = 0; vec < 2; vec++)
			{
					//vec = 1;
					IPAddress remote_server = IPAddress.Parse(server[vec]);
					//IPAddress remote_server = IPAddress.Parse("25.69.221.235");
					//remote_server = IPAddress.Parse("25.8.177.104");
					//remote_server = IPAddress.Parse("192.168.1.130");
					int f_client_port = port[0];
					Console.WriteLine("  before front camera ");
					ThreadCreationProgram f_client = new ThreadCreationProgram(f_camera, remote_server, f_client_port, 0, (byte)vec);
					ThreadStart childref_f_client = new ThreadStart(f_client.thread);

					Thread f_clientThread = new Thread(childref_f_client);
					f_clientThread.Start();

					int b_client_port1 = port[2];
					ThreadCreationProgram b_client = new ThreadCreationProgram(b_camera, remote_server, b_client_port1, 1, (byte)vec);
					ThreadStart childref_b_client = new ThreadStart(b_client.thread);
					Thread b_clientThread = new Thread(childref_b_client);
					b_clientThread.Start();
					try
					{
						// v1  video control for front, back  cameras, motion control  
						VideoControlClients[2 * vec] = new TcpClient(server[vec], port[1]);
						VideoControlStream[2 * vec] = VideoControlClients[2 * vec].GetStream();
					}
					catch (SocketException se)
					{
						Console.WriteLine("SocketException : front Video Control ipaddr {0} port  {1}  reason {2}", server[vec], port[1], se.ToString());

					}
					// back camera

					try
					{

						VideoControlClients[2 * vec + 1] = new TcpClient(server[vec], port[3]);
						VideoControlStream[2 * vec + 1] = VideoControlClients[2 * vec + 1].GetStream();
					}
					catch (SocketException se)
					{
						Console.WriteLine("SocketException : Back Video Control ipaddr {0} port  {1}  reason {2}", server[vec], port[3], se.ToString());

					}

					try
					{
						MotionControlClients[vec] = new TcpClient(server[vec], port[4]);
						MotionControlStream[vec] = MotionControlClients[vec].GetStream();
				    	

			    	}
					catch (SocketException se)
					{
						Console.WriteLine("SocketException : Motion Control ipaddr {0} port  {1}  reason {2}", server[vec], port[4], se.ToString());
					}

			}
			*/

			//joystick = new Joystick_c(MotionControlStream);
		}

		private void zoomout_Paint(Object s, PaintEventArgs e)
		{
			DrawDownArrow(s, e, Brushes.Yellow, Color.Blue, 2);

			//base.Paint(e);
		}

		private void zoomin_Paint(Object s, PaintEventArgs e)
		{
			DrawUPArrow(s, e, Brushes.Yellow, Color.Blue, 2);

			//base.Paint(e);
		}
		private void left_Paint(Object s, PaintEventArgs e)
		{
			DrawLeftArrow(s, e, Brushes.Yellow, Color.Blue, 2);

			//base.Paint(e);
		}

		private void right_Paint(Object s, PaintEventArgs e)
		{
			DrawRightArrow(s, e, Brushes.Yellow, Color.Blue, 2);

			//base.Paint(e);
		}

		private void speedup_Paint(Object s, PaintEventArgs e)
		{
			DrawUPArrow(s, e, Brushes.Yellow, Color.Blue, 2);

			//base.Paint(e);
		}

		private void speeddown_Paint(Object s, PaintEventArgs e)
		{
			DrawDownArrow(s, e, Brushes.Yellow, Color.Blue, 2);

			//base.Paint(e);
		}
		private void foward_Paint(Object s, PaintEventArgs e)
		{
			DrawUPArrow(s, e, Brushes.Yellow, Color.Blue, 2);

			//base.Paint(e);
		}

		private void reverse_Paint(Object s, PaintEventArgs e)
		{
			DrawDownArrow(s, e, Brushes.Yellow, Color.Blue, 2);

			//base.Paint(e);
		}

		public void addCallbackLeft(Button b)
		{
			b.Click += new EventHandler(left_Click);
			b.Paint += new PaintEventHandler(left_Paint);
		}
		public void addCallbackForward(Button b)
		{
			b.Click += new EventHandler(forward_Click);
			b.Paint += new PaintEventHandler(foward_Paint);
		}
		public void addCallbackReverse(Button b)
		{
			b.Click += new EventHandler(reverse_Click);
			b.Paint += new PaintEventHandler(reverse_Paint);
		}
		public void addCallbackStop(Button b)
		{
			b.Click += new EventHandler(stop_Click);
		}

		public void addCallbackStartTraining(Button b)
		{
			b.Click += new EventHandler(startraining_Click);
	
		}

		public void addCallbackStopTraining(Button b)
		{
			b.Click += new EventHandler(stoptraining_Click);
		}

		public void addCallbackReplay(Button b)
		{
			b.Click += new EventHandler(replay_Click);
		}

		public void addCallbackGoback(Button b)
		{
			b.Click += new EventHandler(goback_Click);
		}

		public void addCallbackRight(Button b)
		{
			b.Click += new EventHandler(right_Click);
			b.Paint += new PaintEventHandler(right_Paint);
		}

		public void addCallbackSpeedUp(Button b)
		{
			b.Click += new EventHandler(speedup_Click);
			b.Paint += new PaintEventHandler(speedup_Paint);
		}

		public void addCallbackSpeedDown(Button b)
		{
			b.Click += new EventHandler(speeddown_Click);
			b.Paint += new PaintEventHandler(speeddown_Paint);
		}


		public void addCallbackZoomOut(Button b)
		{
			
			b.Click += new EventHandler(zoomout_Click);
			b.Paint += new PaintEventHandler(zoomout_Paint);
		}

		public void addCallbackZoomIn(Button b)
		{
			b.Click += new EventHandler(zoomin_Click);
			b.Paint += new PaintEventHandler(zoomin_Paint);
		}
		public void addCallbackFrontScreen(MenuItem m)
		{
			m.Click += new EventHandler(frontscreen_Click);
			
		}

		public void addCallbackBackScreen(MenuItem m)
		{
			m.Click += new EventHandler(backscreen_Click);

		}
		public void addCallbackSelectVehicle1(MenuItem m)
		{
			m.Click += new EventHandler(vehicle1_Click);

		}

		private void vehicle1_Click(object sender, EventArgs e)
		{
			active_vehicle = 0;
			for (int k = 0; k< server.Length ; k++)
				{
				 active_vehicles[k] = false;
				 active_screens[2*k] = false;
				 active_screens[2*k+1] = false;
			    }
			active_vehicles[0] = true;
			active_screens[0] = true;
			active_screens[1] = true;
			Console.WriteLine(" ***************** select vehicle 1 ********************************");
		}

		public void addCallbackSelectVehicle2(MenuItem m)
		{
			m.Click += new EventHandler(vehicle2_Click);

		}

		private void vehicle2_Click(object sender, EventArgs e)
		{
			active_vehicle = 1;
			for (int k = 0; k < server.Length; k++)
			{
				active_vehicles[k] = false;
				active_screens[2 * k] = false;
				active_screens[2 * k + 1] = false;
			}
			active_vehicles[1] = true;
			active_screens[2] = true;
			active_screens[3] = true;
			Console.WriteLine(" ***************** select vehicle 1 ********************************");
		}


		

		private void DrawRightArrow (Object s, PaintEventArgs e, Brush ArrowColor, Color LineColor, int LineWidth)
		{
			Button b = (Button)s;
			int offset_x = b.Width / DIVIDE_X;
			int offset_y = b.Height / DIVIDE_Y;
			int w = b.Width - offset_x;
			int h = b.Height - offset_y;
			ArrowPoints = new Point[3];
			ArrowPoints[0] = new Point(offset_x, offset_y);
			ArrowPoints[1] = new Point(offset_x, h);
			ArrowPoints[2] = new Point(w, (h+offset_y)/2);
  
			e.Graphics.FillPolygon(ArrowColor, ArrowPoints);
			using (Pen big_pen = new Pen(LineColor, LineWidth))
			{
				
				
				e.Graphics.DrawPolygon(big_pen, ArrowPoints);
			}
		}
		private void DrawDownArrow(Object s, PaintEventArgs e,Brush ArrowColor, Color LineColor,int LineWidth)
		{
			Button b = (Button)s;
			int offset_x = b.Width / DIVIDE_X;
			int offset_y = b.Height / DIVIDE_Y;
			int w = b.Width - offset_x;
			int h = b.Height - offset_y;
			ArrowPoints = new Point[3];
			ArrowPoints[0] = new Point(offset_x, offset_y);
			ArrowPoints[1] = new Point(w, offset_y);
			ArrowPoints[2] = new Point((w+offset_x)/2, h);
			//ArrowUPPoints[2] = new Point(w, h / 2);

			e.Graphics.FillPolygon(ArrowColor, ArrowPoints);
			using (Pen big_pen = new Pen(LineColor, LineWidth))
			{


				e.Graphics.DrawPolygon(big_pen, ArrowPoints);
			}
		}

		private void DrawLeftArrow(Object s, PaintEventArgs e,Brush ArrowColor, Color LineColor, int LineWidth)
		{
			Button b = (Button)s;
			int offset_x = b.Width / DIVIDE_X;
			int offset_y = b.Height / DIVIDE_Y;
			int w = b.Width - offset_x;
			int h = b.Height - offset_y;
			ArrowPoints = new Point[3];
			//ArrowUPPoints[0] = new Point(w, 0);
			ArrowPoints[0] = new Point(w, offset_y);
			ArrowPoints[1] = new Point(w,h);
			ArrowPoints[2] = new Point(offset_x, (h+offset_y)/2);
			//ArrowUPPoints[1] = new Point(w / 2, h);
			//ArrowUPPoints[2] = new Point(w, h / 2);

			e.Graphics.FillPolygon(ArrowColor, ArrowPoints);
			using (Pen big_pen = new Pen(LineColor, LineWidth))
			{


				e.Graphics.DrawPolygon(big_pen, ArrowPoints);
			}
		}
		private void DrawUPArrow(Object s, PaintEventArgs e,Brush ArrowColor, Color LineColor,int LineWidth)
		{
			Button b = (Button)s;
			int offset_x = b.Width / DIVIDE_X;
			int offset_y = b.Height / DIVIDE_Y;
			int w = b.Width - offset_x;
			int h = b.Height - offset_y;
			ArrowPoints = new Point[3];
			//ArrowUPPoints[0] = new Point(w+OFFSET_X, OFFSET_Y);
			ArrowPoints[0] = new Point(offset_x, h);
			ArrowPoints[1] = new Point(w, h);
			ArrowPoints[2] = new Point((w+offset_x)/2, offset_y);
			//ArrowUPPoints[1] = new Point(w / 2, h);
			//ArrowUPPoints[2] = new Point(w, h / 2);

			e.Graphics.FillPolygon(ArrowColor, ArrowPoints);
			using (Pen big_pen = new Pen(LineColor, LineWidth))
			{


				e.Graphics.DrawPolygon(big_pen, ArrowPoints);
			}
		}

		
		private void left_Click(object sender, EventArgs e)
		{
			data[0] = MV_LEFT;
						
			try
			{
				if (MotionControlStream[active_vehicle] != null)
					MotionControlStream[active_vehicle].Write(data, 0, 4);
				//strm.Write(data, 0, 4);
			}
			catch (ArgumentNullException ane)
			{
				Console.WriteLine("ArgumentNullException : {0}", ane.ToString());
			}
			catch (SocketException se)
			{
				Console.WriteLine("SocketException : {0}", se.ToString());
			}
			catch (Exception ex)
			{
				Console.WriteLine("Unexpected exception : {0}", ex.ToString());
			}


		}


		private void right_Click(object sender, EventArgs e)
		{
			data[0] = MV_RGHT;

			try
			{
				if (MotionControlStream[active_vehicle] != null)
					MotionControlStream[active_vehicle].Write(data, 0, 4);
				//strm.Write(data, 0, 4);
			}
			catch (ArgumentNullException ane)
			{
				Console.WriteLine("ArgumentNullException : {0}", ane.ToString());
			}
			catch (SocketException se)
			{
				Console.WriteLine("SocketException : {0}", se.ToString());
			}
			catch (Exception ex)
			{
				Console.WriteLine("Unexpected exception : {0}", ex.ToString());
			}


		}

		private void speedup_Click(object sender, EventArgs e)
		{
			data[0] = UP_SPEED;
			if (speed + SPEED_INC > MAX_SPEED)
			{
				speed = MAX_SPEED;
				data[1] = MAX_SPEED;
			}
			else
			{
				speed += SPEED_INC;
				data[1] = speed;
			}
			Console.WriteLine("speedup : {0}", data[1]);

			try
			{
				if (MotionControlStream[active_vehicle] != null)
					MotionControlStream[active_vehicle].Write(data, 0, 4);
				//strm.Write(data, 0, 4);
			}
			catch (ArgumentNullException ane)
			{
				Console.WriteLine("ArgumentNullException : {0}", ane.ToString());
			}
			catch (SocketException se)
			{
				Console.WriteLine("SocketException : {0}", se.ToString());
			}
			catch (Exception ex)
			{
				Console.WriteLine("Unexpected exception : {0}", ex.ToString());
			}


		}


		private void speeddown_Click(object sender, EventArgs e)
		{
			data[0] = DN_SPEED;
			speed -= SPEED_INC;
			if (speed < MIN_SPEED)
				speed = MIN_SPEED;
			data[1] = speed;

			try
			{
				if (MotionControlStream[active_vehicle] != null)
					MotionControlStream[active_vehicle].Write(data, 0, 4);
				//strm.Write(data, 0, 4);
			}
			catch (ArgumentNullException ane)
			{
				Console.WriteLine("ArgumentNullException : {0}", ane.ToString());
			}
			catch (SocketException se)
			{
				Console.WriteLine("SocketException : {0}", se.ToString());
			}
			catch (Exception ex)
			{
				Console.WriteLine("Unexpected exception : {0}", ex.ToString());
			}


		}


		private void forward_Click(object sender, EventArgs e)
		{
			data[0] = MV_FRWD;

			try
			{
				if (MotionControlStream[active_vehicle] != null)
					MotionControlStream[active_vehicle].Write(data, 0, 4);
				//strm.Write(data, 0, 4);
			}
			catch (ArgumentNullException ane)
			{
				Console.WriteLine("ArgumentNullException : {0}", ane.ToString());
			}
			catch (SocketException se)
			{
				Console.WriteLine("SocketException : {0}", se.ToString());
			}
			catch (Exception ex)
			{
				Console.WriteLine("Unexpected exception : {0}", ex.ToString());
			}

		}
		private void reverse_Click(object sender, EventArgs e)
		{
			data[0] = MV_BKWD;

			try
			{
				if (MotionControlStream[active_vehicle] != null)
					MotionControlStream[active_vehicle].Write(data, 0, 4);
				//strm.Write(data, 0, 4);
			}
			catch (ArgumentNullException ane)
			{
				Console.WriteLine("ArgumentNullException : {0}", ane.ToString());
			}
			catch (SocketException se)
			{
				Console.WriteLine("SocketException : {0}", se.ToString());
			}
			catch (Exception ex)
			{
				Console.WriteLine("Unexpected exception : {0}", ex.ToString());
			}


		}

		private void stop_Click(object sender, EventArgs e)
		{
			data[0] = STOP;

			try
			{
				if (MotionControlStream[active_vehicle] != null)
					MotionControlStream[active_vehicle].Write(data, 0, 4);
				//strm.Write(data, 0, 4);
			}
			catch (ArgumentNullException ane)
			{
				Console.WriteLine("ArgumentNullException : {0}", ane.ToString());
			}
			catch (SocketException se)
			{
				Console.WriteLine("SocketException : {0}", se.ToString());
			}
			catch (Exception ex)
			{
				Console.WriteLine("Unexpected exception : {0}", ex.ToString());
			}

		}
		private void startraining_Click(object sender, EventArgs e)
		{
			data[0] = START_RECORD;

			try
			{
				if (MotionControlStream[active_vehicle] != null)
					MotionControlStream[active_vehicle].Write(data, 0, 4);
				//strm.Write(data, 0, 4);
			}
			catch (ArgumentNullException ane)
			{
				Console.WriteLine("ArgumentNullException : {0}", ane.ToString());
			}
			catch (SocketException se)
			{
				Console.WriteLine("SocketException : {0}", se.ToString());
			}
			catch (Exception ex)
			{
				Console.WriteLine("Unexpected exception : {0}", ex.ToString());
			}
		}

		private void stoptraining_Click(object sender, EventArgs e)
		{
			data[0] = END_RECORD;

			try
			{
				if (MotionControlStream[active_vehicle] != null)
					MotionControlStream[active_vehicle].Write(data, 0, 4);
				//strm.Write(data, 0, 4);
			}
			catch (ArgumentNullException ane)
			{
				Console.WriteLine("ArgumentNullException : {0}", ane.ToString());
			}
			catch (SocketException se)
			{
				Console.WriteLine("SocketException : {0}", se.ToString());
			}
			catch (Exception ex)
			{
				Console.WriteLine("Unexpected exception : {0}", ex.ToString());
			}
		}

		private void replay_Click(object sender, EventArgs e)
		{
			data[0] = REPLAY_MOTION;

			try
			{
				if (MotionControlStream[active_vehicle] != null)
					MotionControlStream[active_vehicle].Write(data, 0, 4);
				//strm.Write(data, 0, 4);
			}
			catch (ArgumentNullException ane)
			{
				Console.WriteLine("ArgumentNullException : {0}", ane.ToString());
			}
			catch (SocketException se)
			{
				Console.WriteLine("SocketException : {0}", se.ToString());
			}
			catch (Exception ex)
			{
				Console.WriteLine("Unexpected exception : {0}", ex.ToString());
			}
		}

		private void goback_Click(object sender, EventArgs e)
		{
			data[0] = REVERSE_MOTION;

			try
			{
				if (MotionControlStream[active_vehicle] != null)
					MotionControlStream[active_vehicle].Write(data, 0, 4);
				//strm.Write(data, 0, 4);
			}
			catch (ArgumentNullException ane)
			{
				Console.WriteLine("ArgumentNullException : {0}", ane.ToString());
			}
			catch (SocketException se)
			{
				Console.WriteLine("SocketException : {0}", se.ToString());
			}
			catch (Exception ex)
			{
				Console.WriteLine("Unexpected exception : {0}", ex.ToString());
			}
		}


		private void zoomin_Click(object sender, EventArgs e)
		{
			data[0] = ZOOM_CMD;
			Console.WriteLine("Vechicle {0} Screen {1}", active_vehicle,active_screen);
			if (zoom_level + 1 <= ZOOM_MAX)
			{
				zoom_level++;
				data[1] = (byte)zoom_level;
				Console.WriteLine(" zoom in {0}", zoom_level);
				Invalidate();
			}
		   
			try
			{
				if (VideoControlStream[2*active_vehicle+active_screen] != null)
				   VideoControlStream[2*active_vehicle+active_screen].Write(data, 0, 4);
				//strm.Write(data, 0, 4);
			}
			catch (ArgumentNullException ane)
			{
				Console.WriteLine("ArgumentNullException : {0}", ane.ToString());
			}
			catch (SocketException se)
			{
				Console.WriteLine("SocketException : {0}", se.ToString());
			}
			catch (Exception ex)
			{
				Console.WriteLine("Unexpected exception : {0}", ex.ToString());
			}


	}

		private void zoomout_Click(object sender, EventArgs e)
		{
			
			data[0] = ZOOM_CMD;
			Console.WriteLine("Vechicle {0} Screen {1}", active_vehicle,active_screen);
			if (zoom_level -1 >= ZOOM_MIN)
			{
				zoom_level--;
				data[1] = (byte)zoom_level;
				Console.WriteLine(" zoom in {0}", zoom_level);
				Invalidate();
			}

			try
			{
				if (VideoControlStream[2*active_vehicle+active_screen] != null)
					VideoControlStream[2*active_vehicle+active_screen].Write(data, 0, 4);
				//strm.Write(data, 0, 4);
			}
			catch (ArgumentNullException ane)
			{
				Console.WriteLine("ArgumentNullException : {0}", ane.ToString());
			}
			catch (SocketException se)
			{
				Console.WriteLine("SocketException : {0}", se.ToString());
			}
			catch (Exception ex)
			{
				Console.WriteLine("Unexpected exception : {0}", ex.ToString());
			}


		}


		private void DrawBorder(Object s, PaintEventArgs e)
		{
			base.OnPaint(e);
			PictureBox p = (PictureBox)s;
			float thick = (float)border_thickness;
			Console.WriteLine(" line width {0} height {1}", p.Width,p.Height);
			Pen pen = new Pen(Color.Gold, thick);
			
			e.Graphics.DrawLine(pen, 0, 0, p.Width-1, 0);
			e.Graphics.DrawLine(pen, 0, 0, 0, p.Height-1);
			e.Graphics.DrawLine(pen, p.Width-1, p.Height-1, 0, p.Height-1);
			e.Graphics.DrawLine(pen, p.Width-1 ,p.Height-1, p.Width-1, 0);
		}

		private void  frontscreen_border_Paint(Object s, PaintEventArgs e)
		{
			Console.WriteLine("before border paint1 {0}",active_screen);
			if (active_screen == 0)
			{
				Console.WriteLine("border paint1");
				DrawBorder(s, e);
			}

			//base.Paint(e);
		}
		private void backscreen_border_Paint(Object s, PaintEventArgs e)
		{
			Console.WriteLine("border paint2 {0}", active_screen);
			if (active_screen == 1)
			{
				Console.WriteLine("border paint2");
				DrawBorder(s, e);
			}

			//base.Paint(e);
		}
		private void frontscreen_Click(object sender, EventArgs e)
		{

			 active_screen = 0;
			Console.WriteLine("screen 1- {0} {1}", x, active_screen);
			b_camera.Invalidate();
			f_camera.Invalidate();
		}

		private void backscreen_Click(object sender, EventArgs e)
		{
			active_screen = 1;
			Console.WriteLine("screen 2- {0} {1}", x, active_screen);
			//video_screen_idx = 9;
			x++;
			b_camera.Invalidate();
			f_camera.Invalidate();

		}
	}
}
