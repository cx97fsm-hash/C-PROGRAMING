class main:
	def collect_info(self):
		return {
			"name": input("Enter your name: "),
			"age": input("Enter your age: "),
			"percentage": input("Enter your percentage: "),
		}

	def display_info(self, info):
		print("Your info")
		print("Name:", info["name"])
		print("Age:", info["age"])
		print("Percentage:", info["percentage"], "%")

	def run(self):
		self.display_info(self.collect_info())


if __name__ == "__main__":
	main().run()

