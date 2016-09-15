class WelController < ApplicationController

  before_filter :authorize

  def cool
  	puts "Welcome to our survey!"
  end

  def free
  	 puts "Hello"
  end

end