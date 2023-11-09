#include <iostream>
#include <conio.h>
#include <thread>
#include <Windows.h>
using std::cin;
using std::cout;
using std::endl;
using namespace std::chrono_literals;

#define MIN_TANK_VOLUME 20
#define MAX_TANK_VOLUME 120

class Tank
{
	const int VOLUME;
	double fuel;
public:
	const int get_VOLUME()const
	{
		return VOLUME;
	}
	double get_fuel()const
	{
		return fuel;
	}
	void fill(double fuel)
	{
		if (fuel < 0)return;
		if (this->fuel + fuel < VOLUME)this->fuel += fuel;
		else this->fuel = VOLUME;
	}
	double give_fuel(double amount)
	{
		fuel -= amount;
		if (fuel < 0) fuel = 0;
		return fuel;
	}

	Tank(int volume) :VOLUME
	(
		volume < MIN_TANK_VOLUME ? MIN_TANK_VOLUME :
		volume > MAX_TANK_VOLUME ? MAX_TANK_VOLUME :
		volume
	)
	{
		this->fuel = 0;
		cout << "Tank is ready\t" << this << endl;
	}
	~Tank()
	{
		cout << "Tank is over\t" << this << endl;
	}
	void info()const
	{
		cout << "Volume:     " << VOLUME << " liters.\n";
		cout << "Fuel level: " << fuel << " liters.\n";
	}
};


#define MIN_ENGINE_CONSUMPTION 3
#define MAX_ENGINE_CONSUMPTION 30
class Engine
{
	const double CONSUMPTION;
	double consumption_per_second;
	bool is_started;
public:
	double get_CONSUMPTION()const
	{
		return CONSUMPTION;
	}
	double get_consumption_per_second()const
	{
		return consumption_per_second;
	}
	double set_consumption_per_second(int speed)
	{
		if (speed == 0) consumption_per_second = CONSUMPTION * 3e-5;
		else if (speed < 60) consumption_per_second = CONSUMPTION * 3e-5 * 20 / 3;
		else if (speed < 100) consumption_per_second = CONSUMPTION * 3e-5 * 14 / 3;
		else if (speed < 140) consumption_per_second = CONSUMPTION * 3e-5 * 20 / 3;
		else if (speed < 200) consumption_per_second = CONSUMPTION * 3e-5 * 25 / 3;
		else if (speed > 200) consumption_per_second = CONSUMPTION * 3e-5 * 10;
		return consumption_per_second;
	}
	void start()
	{
		is_started = true;
	}
	void stop()
	{
		is_started = false;
	}
	bool started()const
	{
		return is_started;
	}
	Engine(double consumption) :CONSUMPTION
	(
		consumption < MIN_ENGINE_CONSUMPTION ? MIN_ENGINE_CONSUMPTION :
		consumption > MAX_ENGINE_CONSUMPTION ? MAX_ENGINE_CONSUMPTION :
		consumption
	)
	{
		consumption_per_second = CONSUMPTION * 3e-5;
		is_started = false;
		cout << "Engine is ready\t" << this << endl;
	}
	~Engine()
	{
		cout << "Engine is over\t" << this << endl;
	}

	void info()const
	{
		cout << "Consumption:     " << get_CONSUMPTION() << " liters.\n";
		cout << "Consumption/sec: " << get_consumption_per_second() << " liters.\n";
	}
};

#define MAX_SPEED_LOW_LEVEL 50
#define MAX_SPEED_HIGH_LEVEL 450
class Car
{
	Engine engine;
	Tank tank;
	int MAX_SPEED;
	int speed;
	int acceleration;
	bool driver_inside;
	struct ControlThreads
	{
		std::thread panel_thread;
		std::thread engine_idle_thread;
		std::thread free_wheeling_thread;
	}control_threads;
public:
	int get_MAX_SPEED()const
	{
		return MAX_SPEED;
	}
	int get_speed()const
	{
		return speed;
	}
	Car(int max_speed, int acceleration = 10, double consumption = 10, int volume = 40) :MAX_SPEED
	(
		max_speed < MAX_SPEED_LOW_LEVEL ? MAX_SPEED_LOW_LEVEL :
		max_speed > MAX_SPEED_HIGH_LEVEL ? MAX_SPEED_HIGH_LEVEL :
		max_speed
	), engine(consumption), tank(volume)
	{
		this->speed = 0;
		this->acceleration = acceleration;
		driver_inside = false;
		cout << "Your car is ready, press Enter to get in\t" << this << endl;
	}
	~Car()
	{
		cout << "Your car is over\t" << this << endl;
	}
	void fill(double fuel)
	{
		tank.fill(fuel);
	}
	void get_in()
	{
		if (speed == 0)
		{
			driver_inside = true;
			control_threads.panel_thread = std::thread(&Car::panel, this);
		}
	}
	void get_out()
	{
		if (speed == 0)
		{
			driver_inside = false;
			if (control_threads.panel_thread.joinable()) control_threads.panel_thread.join();
			system("CLS");
			cout << "You are out" << endl;
		}
	}
	void start()
	{
		if (tank.get_fuel() && driver_inside)
		{
			engine.start();
			control_threads.engine_idle_thread = std::thread(&Car::engine_idle, this);
		}
	}
	void stop()
	{
		engine.stop();
		if (control_threads.engine_idle_thread.joinable()) control_threads.engine_idle_thread.join();
	}
	void accelerate()
	{
		if (driver_inside && engine.started())
		{
			speed += acceleration;
			if (speed > MAX_SPEED)speed = MAX_SPEED;
			if (!control_threads.free_wheeling_thread.joinable())
				control_threads.free_wheeling_thread = std::thread(&Car::free_wheeling, this);
			std::this_thread::sleep_for(1s);
		}
	}
	void slow_down()
	{
		if (driver_inside)
		{
			speed -= acceleration;
			if (speed < 0)speed = 0;
			std::this_thread::sleep_for(1s);
		}
	}
	void control()
	{
		char key;
		do {
			key = 0;
			if (_kbhit())key = _getch();
			switch (key)
			{
			case 'F':
			case 'f':
			{
				if (driver_inside)
				{
					cout << "First of all get out of your car" << endl;
					break;
				}
				double fuel;
				cout << "¬ведите уровень топлива: "; cin >> fuel;
				fill(fuel);
			}
			break;
			case 'W':
			case 'w':
			{
				if (driver_inside && engine.started())
					accelerate();
			}
			break;
			case 'S':
			case 's':
			{
				if (driver_inside)slow_down();
			}
			break;
			case 13:
			{
				if (driver_inside) get_out();
				else get_in();
			}
			break;
			case 'I':
			case 'i':
			{
				if (engine.started()) stop();
				else start();
			}
			break;
			case 27:
				speed = 0;
				stop();
				get_out();
			}
			if (tank.get_fuel() == 0 && control_threads.engine_idle_thread.joinable())
			{
				engine.stop();
				control_threads.engine_idle_thread.join();
			}
			if (speed == 0 && control_threads.free_wheeling_thread.joinable())
				control_threads.free_wheeling_thread.join();
		} while (key != 27);
	}
	void engine_idle()
	{
		while (engine.started() && tank.give_fuel(engine.set_consumption_per_second(speed)))
		{
			std::this_thread::sleep_for(1s);
		}
	}
	void free_wheeling()
	{
		while (speed > 0)
		{
			speed--;
			if (speed < 0)speed = 0;
			std::this_thread::sleep_for(1s);
		}
	}
	void panel()const
	{
		while (driver_inside)
		{
			system("CLS");
			for (int i = 0; i < speed / 3; i++)cout << "|";
			cout << endl;
			cout << "Speed:\t" << speed << " km/h.\n";
			cout << "Fuel level: " << tank.get_fuel() << " liters.\t";
			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			if (tank.get_fuel() < 5)
			{
				SetConsoleTextAttribute(hConsole, 0xCF);
				cout << " LOW FUEL ";
				SetConsoleTextAttribute(hConsole, 0x07);
			}
			cout << endl;
			cout << "Engine is " << (engine.started() ? "started" : "stopped") << endl;
			cout << "Consumption per second:\t" << engine.get_consumption_per_second() << " liters.\n";
			std::this_thread::sleep_for(100ms);
		}
	}
	void info()const
	{
		engine.info();
		tank.info();
	}
};

//#define TANK_CHECK
//#define ENGINE_CHECK
void main()
{
	setlocale(LC_ALL, "");

#ifdef TANK_CHECK
	Tank tank(12);
	double fuel;
	do {
		cout << "¬ведите уровень топлива: "; cin >> fuel;
		tank.fill(fuel);
		tank.info();
} while (fuel != 0);
#endif // TANK_CHECK

#ifdef ENGINE_CHECK
Engine engine(10);
engine.info();
#endif // ENGINE_CHECK

Car bmw(250);
//bmw.info();
bmw.control();
}