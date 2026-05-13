-- Prototype Supabase setup for the ESP8266 GPS tracker.
-- Run this in the Supabase SQL Editor.

create extension if not exists pgcrypto;

create table if not exists public.gps_locations (
  id uuid primary key default gen_random_uuid(),
  device_id text not null,
  latitude double precision not null,
  longitude double precision not null,
  created_at timestamptz not null default now()
);

create index if not exists idx_gps_locations_device_created_at
  on public.gps_locations (device_id, created_at desc);

drop view if exists public.gps_latest_locations;

create view public.gps_latest_locations as
select distinct on (device_id)
  device_id,
  latitude,
  longitude,
  created_at as recorded_at
from public.gps_locations
order by device_id, created_at desc;

alter table public.gps_locations enable row level security;

grant insert on public.gps_locations to anon;
grant select on public.gps_locations to anon;
grant select on public.gps_latest_locations to anon;

do $$
begin
  if not exists (
    select 1
    from pg_publication_tables
    where pubname = 'supabase_realtime'
      and schemaname = 'public'
      and tablename = 'gps_locations'
  ) then
    alter publication supabase_realtime add table public.gps_locations;
  end if;
end
$$;

drop policy if exists "anon can insert gps rows" on public.gps_locations;
create policy "anon can insert gps rows"
  on public.gps_locations
  for insert
  to anon
  with check (true);

drop policy if exists "anon can read gps rows" on public.gps_locations;
create policy "anon can read gps rows"
  on public.gps_locations
  for select
  to anon
  using (true);
