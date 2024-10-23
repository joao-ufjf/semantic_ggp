import asyncio
import aiofiles
import datetime
import argparse

start = datetime.datetime.now()

async def read_file(filename):
    async with aiofiles.open(filename, 'r') as file:
        return await file.readlines()

async def write_to_file(filename, content):
    async with aiofiles.open(filename, 'a') as file:
        await file.write(content)

async def do_job(c, total, command, executed_file, sem):
    async with sem:  # Control the number of simultaneous jobs
        print(f"{c/total:.2%} {command}")
        proc = await asyncio.create_subprocess_shell(command, stdout=asyncio.subprocess.PIPE)
        output, _ = await proc.communicate()

        await write_to_file(executed_file, f"{command.strip()}\n")

async def main(commands_file, executed_file, max_concurrent):
    sem = asyncio.Semaphore(max_concurrent)
    commands = await read_file(commands_file)
    executed = await read_file(executed_file)

    tasks = []
    total = len(commands)
    for c, command in enumerate(commands):
        if command not in executed:
            task = asyncio.create_task(do_job(c, total, command, executed_file, sem))
            tasks.append(task)

    await asyncio.gather(*tasks)

    print(datetime.datetime.now() - start)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Run commands asynchronously.")
    parser.add_argument('commands_file', help="The file containing the list of commands to execute.")
    parser.add_argument('executed_file', help="The file to log executed commands.")
    parser.add_argument('--max-concurrent', type=int, default=14, help="The maximum number of concurrent commands.")

    args = parser.parse_args()

    # Run the main function with the provided files and max concurrent tasks
    asyncio.run(main(args.commands_file, args.executed_file, args.max_concurrent))
