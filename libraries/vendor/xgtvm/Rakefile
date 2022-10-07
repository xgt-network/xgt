require 'json'

def which?(cmd_name)
  return false
  %x([ -x "$(command -v #{cmd_name})" ] && echo yes).length > 0
end

def xgtvm_eval(string)
  # cmd = %(cd build && ./xgtvmd -e'#{string}')
  cmd = %(cd build && echo '#{string}' | ./xgtvmd)
  $stderr.puts(cmd)
  result = %x(#{cmd})
  JSON.load(result)
end

desc 'removes build artifacts'
task :clean do
  sh %(rm -rf build)
end

desc 'configures the project'
task :configure do
  sh %(mkdir -p build)
  sh %(cd build && cmake ..)
end

desc 'builds the project'
task :make do
  sh %(mkdir -p build)
  sh %(cd build && cmake --build .)
end

desc 'runs the project'
task :test do
  sh %(cd build && ./xgtvm_tests)

  # result = xgtvm_eval('60 03 60 08 01 00')
  result = xgtvm_eval('42 00')
  p result
  # stack = result.fetch('finalState').fetch('stack')
  # raise 'fail' unless stack == ['11']

  # result = xgtvm_eval('60 00 60 01 5B 60 04 57 00')
  # p result
  # stack = result.fetch('finalState').fetch('stack')
  # raise 'fail' unless stack == []

  # result = xgtvm_eval('42 00')
  # p result
  # stack = result.fetch('finalState').fetch('stack')
  # raise 'fail' unless stack == []
end

task :default => [:configure, :make, :test]
