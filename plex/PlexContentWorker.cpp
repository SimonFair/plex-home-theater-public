/*
 *  Copyright (C) 2011 Plex, Inc.   
 *      Author: Elan Feingold
 */

#include "PlexContentWorker.h"

/////////////////////////////////////////////////////////////////////////////////////////
PlexContentWorkerPtr PlexContentWorkerManager::enqueue(int targetWindow, const string& url, int contextID)
{
  recursive_mutex::scoped_lock lk(m_mutex);
  
  // Create the worker and add to the map.
  PlexContentWorkerPtr worker = PlexContentWorkerPtr(new PlexContentWorker(this, m_workerID++, targetWindow, url, contextID));
  m_pendingWorkers[worker->getID()] = worker;
  
  // Run in a separate thread.
  thread t(boost::bind(&PlexContentWorker::run, worker.get()));
  t.detach();
  
  return worker;
}

/////////////////////////////////////////////////////////////////////////////////////////
void PlexContentWorkerManager::cancelPending()
{
  recursive_mutex::scoped_lock lk(m_mutex);
  
  typedef pair<int, PlexContentWorkerPtr> int_worker_pair;
  BOOST_FOREACH(int_worker_pair pair, m_pendingWorkers)
    pair.second->cancel();
}
